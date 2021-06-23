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

#ifndef BIXD_RPC_CONTEXT_H_INCLUDED
#define BIXD_RPC_CONTEXT_H_INCLUDED

#include <bixd/core/Config.h>
#include <bixd/core/JobQueue.h>
#include <bixd/net/InfoSub.h>
#include <bixd/rpc/Role.h>

#include <bixd/beast/utility/Journal.h>

namespace bixd {

class Application;
class NetworkOPs;
class LedgerMaster;

namespace RPC {

/** The context of information needed to call an RPC. */
struct Context
{
    beast::Journal const j;
    Application& app;
    Resource::Charge& loadType;
    NetworkOPs& netOps;
    LedgerMaster& ledgerMaster;
    Resource::Consumer& consumer;
    Role role;
    std::shared_ptr<JobQueue::Coro> coro{};
    InfoSub::pointer infoSub{};
    unsigned int apiVersion;
};

struct JsonContext : public Context
{
    /**
     * Data passed in from HTTP headers.
     */
    struct Headers
    {
        boost::string_view user;
        boost::string_view forwardedFor;
    };

    Json::Value params;

    Headers headers{};
};

template <class RequestType>
struct GRPCContext : public Context
{
    RequestType params;
};

}  // namespace RPC
}  // namespace bixd

#endif
