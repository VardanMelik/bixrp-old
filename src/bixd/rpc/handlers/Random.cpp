//------------------------------------------------------------------------------
/*
    This file is part of bixd
    Copyright (c) 2012-2014 bixd Labs Inc.

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

#include <bixd/basics/base_uint.h>
#include <bixd/beast/utility/rngfill.h>
#include <bixd/crypto/csprng.h>
#include <bixd/json/json_value.h>
#include <bixd/net/RPCErr.h>
#include <bixd/protocol/ErrorCodes.h>
#include <bixd/protocol/jss.h>

namespace bixd {

namespace RPC {
struct JsonContext;
}

// Result:
// {
//   random: <uint256>
// }
Json::Value
doRandom(RPC::JsonContext& context)
{
    // TODO(tom): the try/catch is almost certainly redundant, we catch at the
    // top level too.
    try
    {
        uint256 rand;
        beast::rngfill(rand.begin(), rand.size(), crypto_prng());

        Json::Value jvResult;
        jvResult[jss::random] = to_string(rand);
        return jvResult;
    }
    catch (std::exception const&)
    {
        return rpcError(rpcINTERNAL);
    }
}

}  // namespace bixd
