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
#include <bixd/app/misc/NetworkOPs.h>
#include <bixd/core/Config.h>
#include <bixd/net/RPCErr.h>
#include <bixd/protocol/ErrorCodes.h>
#include <bixd/protocol/Indexes.h>
#include <bixd/protocol/jss.h>
#include <bixd/rpc/Context.h>

#include <mutex>

namespace bixd {

Json::Value
doLedgerAccept(RPC::JsonContext& context)
{
    std::unique_lock lock{context.app.getMasterMutex()};
    Json::Value jvResult;

    if (!context.app.config().standalone() || context.app.config().reporting())
    {
        jvResult[jss::error] = "notStandAlone";
    }
    else
    {
        context.netOps.acceptLedger();

        jvResult[jss::ledger_current_index] =
            context.ledgerMaster.getCurrentLedgerIndex();
    }

    return jvResult;
}

}  // namespace bixd
