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

#ifndef BIXD_APP_LEDGER_LEDGERCLEANER_H_INCLUDED
#define BIXD_APP_LEDGER_LEDGERCLEANER_H_INCLUDED

#include <bixd/app/main/Application.h>
#include <bixd/beast/utility/Journal.h>
#include <bixd/beast/utility/PropertyStream.h>
#include <bixd/core/Stoppable.h>
#include <bixd/json/json_value.h>
#include <memory>

namespace bixd {
namespace detail {

/** Check the ledger/transaction databases to make sure they have continuity */
class LedgerCleaner : public Stoppable, public beast::PropertyStream::Source
{
protected:
    explicit LedgerCleaner(Stoppable& parent);

public:
    /** Destroy the object. */
    virtual ~LedgerCleaner() = 0;

    /** Start a long running task to clean the ledger.
        The ledger is cleaned asynchronously, on an implementation defined
        thread. This function call does not block. The long running task
        will be stopped if the Stoppable stops.

        Thread safety:
            Safe to call from any thread at any time.

        @param parameters A Json object with configurable parameters.
    */
    virtual void
    doClean(Json::Value const& parameters) = 0;
};

std::unique_ptr<LedgerCleaner>
make_LedgerCleaner(Application& app, Stoppable& parent, beast::Journal journal);

}  // namespace detail
}  // namespace bixd

#endif
