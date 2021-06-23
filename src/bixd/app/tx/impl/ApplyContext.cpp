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

#include <bixd/app/tx/impl/ApplyContext.h>
#include <bixd/app/tx/impl/InvariantCheck.h>
#include <bixd/app/tx/impl/Transactor.h>
#include <bixd/basics/Log.h>
#include <bixd/json/to_string.h>
#include <bixd/protocol/Feature.h>
#include <bixd/protocol/Indexes.h>
#include <cassert>

namespace bixd {

ApplyContext::ApplyContext(
    Application& app_,
    OpenView& base,
    STTx const& tx_,
    TER preclaimResult_,
    FeeUnit64 baseFee_,
    ApplyFlags flags,
    beast::Journal journal_)
    : app(app_)
    , tx(tx_)
    , preclaimResult(preclaimResult_)
    , baseFee(baseFee_)
    , journal(journal_)
    , base_(base)
    , flags_(flags)
{
    view_.emplace(&base_, flags_);
}

void
ApplyContext::discard()
{
    view_.emplace(&base_, flags_);
}

void
ApplyContext::apply(TER ter)
{
    view_->apply(base_, tx, ter, journal);
}

std::size_t
ApplyContext::size()
{
    return view_->size();
}

void
ApplyContext::visit(std::function<void(
                        uint256 const&,
                        bool,
                        std::shared_ptr<SLE const> const&,
                        std::shared_ptr<SLE const> const&)> const& func)
{
    view_->visit(base_, func);
}

TER
ApplyContext::failInvariantCheck(TER const result)
{
    // If we already failed invariant checks before and we are now attempting to
    // only charge a fee, and even that fails the invariant checks something is
    // very wrong. We switch to tefINVARIANT_FAILED, which does NOT get included
    // in a ledger.

    return (result == tecINVARIANT_FAILED || result == tefINVARIANT_FAILED)
        ? TER{tefINVARIANT_FAILED}
        : TER{tecINVARIANT_FAILED};
}

template <std::size_t... Is>
TER
ApplyContext::checkInvariantsHelper(
    TER const result,
    BIXRPAmount const fee,
    std::index_sequence<Is...>)
{
    try
    {
        auto checkers = getInvariantChecks();

        // call each check's per-entry method
        visit([&checkers](
                  uint256 const& index,
                  bool isDelete,
                  std::shared_ptr<SLE const> const& before,
                  std::shared_ptr<SLE const> const& after) {
            (..., std::get<Is>(checkers).visitEntry(isDelete, before, after));
        });

        // Note: do not replace this logic with a `...&&` fold expression.
        // The fold expression will only run until the first check fails (it
        // short-circuits). While the logic is still correct, the log
        // message won't be. Every failed invariant should write to the log,
        // not just the first one.
        std::array<bool, sizeof...(Is)> finalizers{
            {std::get<Is>(checkers).finalize(
                tx, result, fee, *view_, journal)...}};

        // call each check's finalizer to see that it passes
        if (!std::all_of(
                finalizers.cbegin(), finalizers.cend(), [](auto const& b) {
                    return b;
                }))
        {
            JLOG(journal.fatal())
                << "Transaction has failed one or more invariants: "
                << to_string(tx.getJson(JsonOptions::none));

            return failInvariantCheck(result);
        }
    }
    catch (std::exception const& ex)
    {
        JLOG(journal.fatal())
            << "Transaction caused an exception in an invariant"
            << ", ex: " << ex.what()
            << ", tx: " << to_string(tx.getJson(JsonOptions::none));

        return failInvariantCheck(result);
    }

    return result;
}

TER
ApplyContext::checkInvariants(TER const result, BIXRPAmount const fee)
{
    assert(isTesSuccess(result) || isTecClaim(result));

    return checkInvariantsHelper(
        result,
        fee,
        std::make_index_sequence<std::tuple_size<InvariantChecks>::value>{});
}

}  // namespace bixd
