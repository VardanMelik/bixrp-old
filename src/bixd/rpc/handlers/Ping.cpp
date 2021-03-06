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

#include <bixd/json/json_value.h>
#include <bixd/protocol/jss.h>
#include <bixd/rpc/Context.h>
#include <bixd/rpc/Role.h>

namespace bixd {

namespace RPC {
struct JsonContext;
}  // namespace RPC

Json::Value
doPing(RPC::JsonContext& context)
{
    Json::Value ret(Json::objectValue);
    switch (context.role)
    {
        case Role::ADMIN:
            ret[jss::role] = "admin";
            break;
        case Role::IDENTIFIED:
            ret[jss::role] = "identified";
            ret[jss::username] = context.headers.user.to_string();
            if (context.headers.forwardedFor.size())
                ret[jss::ip] = context.headers.forwardedFor.to_string();
            break;
        case Role::PROXY:
            ret[jss::role] = "proxied";
            ret[jss::ip] = context.headers.forwardedFor.to_string();
        default:;
    }

    // This is only accessible on ws sessions.
    if (context.infoSub)
    {
        if (context.infoSub->getConsumer().isUnlimited())
            ret[jss::unlimited] = true;
    }

    return ret;
}

}  // namespace bixd
