//------------------------------------------------------------------------------
/*
    This file is part of bixd
    Copyright (c) 2012, 2020 bixd Labs Inc.

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

#ifndef BIXD_APP_LEDGER_LEDGERREPLAYMSGHANDLER_H_INCLUDED
#define BIXD_APP_LEDGER_LEDGERREPLAYMSGHANDLER_H_INCLUDED

#include <bixd/beast/utility/Journal.h>
#include <bixd/protocol/messages.h>

namespace bixd {
class Application;
class LedgerReplayer;

class LedgerReplayMsgHandler final
{
public:
    LedgerReplayMsgHandler(Application& app, LedgerReplayer& replayer);
    ~LedgerReplayMsgHandler() = default;

    /**
     * Process TMProofPathRequest and return TMProofPathResponse
     * @note check has_error() and error() of the response for error
     */
    protocol::TMProofPathResponse
    processProofPathRequest(
        std::shared_ptr<protocol::TMProofPathRequest> const& msg);

    /**
     * Process TMProofPathResponse
     * @return false if the response message has bad format or bad data;
     *         true otherwise
     */
    bool
    processProofPathResponse(
        std::shared_ptr<protocol::TMProofPathResponse> const& msg);

    /**
     * Process TMReplayDeltaRequest and return TMReplayDeltaResponse
     * @note check has_error() and error() of the response for error
     */
    protocol::TMReplayDeltaResponse
    processReplayDeltaRequest(
        std::shared_ptr<protocol::TMReplayDeltaRequest> const& msg);

    /**
     * Process TMReplayDeltaResponse
     * @return false if the response message has bad format or bad data;
     *         true otherwise
     */
    bool
    processReplayDeltaResponse(
        std::shared_ptr<protocol::TMReplayDeltaResponse> const& msg);

private:
    Application& app_;
    LedgerReplayer& replayer_;
    beast::Journal journal_;
};

}  // namespace bixd

#endif
