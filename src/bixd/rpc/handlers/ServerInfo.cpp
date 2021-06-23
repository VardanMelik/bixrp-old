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

#include <bixd/app/main/Application.h>
#include <bixd/app/misc/NetworkOPs.h>
#include <bixd/app/reporting/P2pProxy.h>
#include <bixd/json/json_value.h>
#include <bixd/net/RPCErr.h>
#include <bixd/protocol/jss.h>
#include <bixd/rpc/Context.h>
#include <bixd/rpc/Role.h>
#include <bixd/rpc/impl/TransactionSign.h>

namespace bixd {

Json::Value
doServerInfo(RPC::JsonContext& context)
{
    Json::Value ret(Json::objectValue);

    ret[jss::info] = context.netOps.getServerInfo(
        true,
        context.role == Role::ADMIN,
        context.params.isMember(jss::counters) &&
            context.params[jss::counters].asBool());

    if (context.app.config().reporting())
    {
        Json::Value const proxied = forwardToP2p(context);
        auto const lf = proxied[jss::result][jss::info][jss::load_factor];
        ret[jss::info][jss::load_factor] = lf.isNull() ? 1 : lf;
    }
    return ret;
}

}  // namespace bixd
