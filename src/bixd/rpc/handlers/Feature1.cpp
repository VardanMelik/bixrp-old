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

#include <bixd/app/ledger/LedgerMaster.h>
#include <bixd/app/main/Application.h>
#include <bixd/app/misc/AmendmentTable.h>
#include <bixd/net/RPCErr.h>
#include <bixd/protocol/ErrorCodes.h>
#include <bixd/protocol/jss.h>
#include <bixd/rpc/Context.h>

namespace bixd {

// {
//   feature : <feature>
//   vetoed : true/false
// }
Json::Value
doFeature(RPC::JsonContext& context)
{
    if (context.app.config().reporting())
        return rpcError(rpcREPORTING_UNSUPPORTED);

    // Get majority amendment status
    majorityAmendments_t majorities;

    if (auto const valLedger = context.ledgerMaster.getValidatedLedger())
        majorities = getMajorityAmendments(*valLedger);

    auto& table = context.app.getAmendmentTable();

    if (!context.params.isMember(jss::feature))
    {
        auto features = table.getJson();

        for (auto const& [h, t] : majorities)
        {
            features[to_string(h)][jss::majority] =
                t.time_since_epoch().count();
        }

        Json::Value jvReply = Json::objectValue;
        jvReply[jss::features] = features;
        return jvReply;
    }

    auto feature = table.find(context.params[jss::feature].asString());

    if (!feature && !feature.parseHex(context.params[jss::feature].asString()))
        return rpcError(rpcBAD_FEATURE);

    if (context.params.isMember(jss::vetoed))
    {
        if (context.params[jss::vetoed].asBool())
            table.veto(feature);
        else
            table.unVeto(feature);
    }

    Json::Value jvReply = table.getJson(feature);

    auto m = majorities.find(feature);
    if (m != majorities.end())
        jvReply[jss::majority] = m->second.time_since_epoch().count();

    return jvReply;
}

}  // namespace bixd
