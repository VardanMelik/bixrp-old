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

#ifndef BIXD_APP_MISC_FEEVOTE_H_INCLUDED
#define BIXD_APP_MISC_FEEVOTE_H_INCLUDED

#include <bixd/basics/BasicConfig.h>
#include <bixd/ledger/ReadView.h>
#include <bixd/protocol/STValidation.h>
#include <bixd/protocol/SystemParameters.h>
#include <bixd/shamap/SHAMap.h>

namespace bixd {

/** Manager to process fee votes. */
class FeeVote
{
public:
    /** Fee schedule to vote for.
        During voting ledgers, the FeeVote logic will try to move towards
        these values when injecting fee-setting transactions.
        A default-constructed Setup contains recommended values.
    */
    struct Setup
    {
        /** The cost of a reference transaction in drops. */
        BIXRPAmount reference_fee{10};

        /** The cost of a reference transaction in fee units. */
        static constexpr FeeUnit32 reference_fee_units{10};

        /** The account reserve requirement in drops. */
        BIXRPAmount account_reserve{20 * DROPS_PER_BIXRP};

        /** The per-owned item reserve requirement in drops. */
        BIXRPAmount owner_reserve{5 * DROPS_PER_BIXRP};
    };

    virtual ~FeeVote() = default;

    /** Add local fee preference to validation.

        @param lastClosedLedger
        @param baseValidation
    */
    virtual void
    doValidation(Fees const& lastFees, STValidation& val) = 0;

    /** Cast our local vote on the fee.

        @param lastClosedLedger
        @param initialPosition
    */
    virtual void
    doVoting(
        std::shared_ptr<ReadView const> const& lastClosedLedger,
        std::vector<std::shared_ptr<STValidation>> const& parentValidations,
        std::shared_ptr<SHAMap> const& initialPosition) = 0;
};

/** Build FeeVote::Setup from a config section. */
FeeVote::Setup
setup_FeeVote(Section const& section);

/** Create an instance of the FeeVote logic.
    @param setup The fee schedule to vote for.
    @param journal Where to log.
*/
std::unique_ptr<FeeVote>
make_FeeVote(FeeVote::Setup const& setup, beast::Journal journal);

}  // namespace bixd

#endif
