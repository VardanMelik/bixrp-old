//------------------------------------------------------------------------------
/*
    This file is part of bixd
    Copyright (c) 2012, 2013 bixd Labs Inc.

    Permission to use, copy, modify, and/or distribute this software for any
    purpose  with  or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE  SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH  REGARD  TO  THIS  SOFTWARE  INCLUDING  ALL  IMPLIED  WARRANTIES  OF
    MERCHANTABILITY  AND  FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY  SPECIAL ,  DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER  RESULTING  FROM  LOSS  OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION  OF  CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
//==============================================================================

#include <bixd/app/misc/ValidatorKeys.h>

#include <bixd/app/misc/Manifest.h>
#include <bixd/basics/Log.h>
#include <bixd/basics/base64.h>
#include <bixd/core/Config.h>
#include <bixd/core/ConfigSections.h>

namespace bixd {
ValidatorKeys::ValidatorKeys(Config const& config, beast::Journal j)
{
    if (config.exists(SECTION_VALIDATOR_TOKEN) &&
        config.exists(SECTION_VALIDATION_SEED))
    {
        configInvalid_ = true;
        JLOG(j.fatal()) << "Cannot specify both [" SECTION_VALIDATION_SEED
                           "] and [" SECTION_VALIDATOR_TOKEN "]";
        return;
    }

    if (config.exists(SECTION_VALIDATOR_TOKEN))
    {
        // token is non-const so it can be moved from
        if (auto token = loadValidatorToken(
                config.section(SECTION_VALIDATOR_TOKEN).lines()))
        {
            auto const pk =
                derivePublicKey(KeyType::secp256k1, token->validationSecret);
            auto const m = deserializeManifest(base64_decode(token->manifest));

            if (!m || pk != m->signingKey)
            {
                configInvalid_ = true;
                JLOG(j.fatal())
                    << "Invalid token specified in [" SECTION_VALIDATOR_TOKEN
                       "]";
            }
            else
            {
                secretKey = token->validationSecret;
                publicKey = pk;
                nodeID = calcNodeID(m->masterKey);
                manifest = std::move(token->manifest);
            }
        }
        else
        {
            configInvalid_ = true;
            JLOG(j.fatal())
                << "Invalid token specified in [" SECTION_VALIDATOR_TOKEN "]";
        }
    }
    else if (config.exists(SECTION_VALIDATION_SEED))
    {
        auto const seed = parseBase58<Seed>(
            config.section(SECTION_VALIDATION_SEED).lines().front());
        if (!seed)
        {
            configInvalid_ = true;
            JLOG(j.fatal())
                << "Invalid seed specified in [" SECTION_VALIDATION_SEED "]";
        }
        else
        {
            secretKey = generateSecretKey(KeyType::secp256k1, *seed);
            publicKey = derivePublicKey(KeyType::secp256k1, secretKey);
            nodeID = calcNodeID(publicKey);
        }
    }
}
}  // namespace bixd
