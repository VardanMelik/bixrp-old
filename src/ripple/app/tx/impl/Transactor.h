//------------------------------------------------------------------------------
/*
    This file is part of rippled: https://github.com/ripple/rippled
    Copyright (c) 2012, 2013 Ripple Labs Inc.

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

#ifndef RIPPLE_APP_TX_TRANSACTOR_H_INCLUDED
#define RIPPLE_APP_TX_TRANSACTOR_H_INCLUDED

#include <ripple/app/tx/applySteps.h>
#include <ripple/app/tx/impl/ApplyContext.h>
#include <ripple/basics/XRPAmount.h>
#include <ripple/beast/utility/Journal.h>
#include <boost/optional.hpp>

namespace ripple {

/** State information when preflighting a tx. */
struct PreflightContext
{
public:
    Application& app;
    STTx const& tx;
    Rules const rules;
    ApplyFlags flags;
    beast::Journal const j;

    PreflightContext(
        Application& app_,
        STTx const& tx_,
        Rules const& rules_,
        ApplyFlags flags_,
        beast::Journal j_);

    PreflightContext&
    operator=(PreflightContext const&) = delete;
};

/** State information when determining if a tx is likely to claim a fee. */
struct PreclaimContext
{
public:
    Application& app;
    ReadView const& view;
    TER preflightResult;
    STTx const& tx;
    ApplyFlags flags;
    beast::Journal const j;

    PreclaimContext(
        Application& app_,
        ReadView const& view_,
        TER preflightResult_,
        STTx const& tx_,
        ApplyFlags flags_,
        beast::Journal j_ = beast::Journal{beast::Journal::getNullSink()})
        : app(app_)
        , view(view_)
        , preflightResult(preflightResult_)
        , tx(tx_)
        , flags(flags_)
        , j(j_)
    {
    }

    PreclaimContext&
    operator=(PreclaimContext const&) = delete;
};

class TxConsequences;
struct PreflightResult;

class Transactor
{
protected:
    ApplyContext& ctx_;
    beast::Journal const j_;

    AccountID const account_;
    BIXRPAmount mPriorBalance;   // Balance before fees.
    BIXRPAmount mSourceBalance;  // Balance after fees.

    virtual ~Transactor() = default;
    Transactor(Transactor const&) = delete;
    Transactor&
    operator=(Transactor const&) = delete;

public:
    enum ConsequencesFactoryType { Normal, Blocker, Custom };
    /** Process the transaction. */
    std::pair<TER, bool>
    operator()();

    ApplyView&
    view()
    {
        return ctx_.view();
    }

    ApplyView const&
    view() const
    {
        return ctx_.view();
    }

    /////////////////////////////////////////////////////
    /*
    These static functions are called from invoke_preclaim<Tx>
    using name hiding to accomplish compile-time polymorphism,
    so derived classes can override for different or extra
    functionality. Use with care, as these are not really
    virtual and so don't have the compiler-time protection that
    comes with it.
    */

    static NotTEC
    checkSeqProxy(ReadView const& view, STTx const& tx, beast::Journal j);

    static NotTEC
    checkPriorTxAndLastLedger(PreclaimContext const& ctx);

    static TER
    checkFee(PreclaimContext const& ctx, FeeUnit64 baseFee);

    static NotTEC
    checkSign(PreclaimContext const& ctx);

    // Returns the fee in fee units, not scaled for load.
    static FeeUnit64
    calculateBaseFee(ReadView const& view, STTx const& tx);

    static TER
    preclaim(PreclaimContext const& ctx)
    {
        // Most transactors do nothing
        // after checkSeq/Fee/Sign.
        return tesSUCCESS;
    }
    /////////////////////////////////////////////////////

    // Interface used by DeleteAccount
    static TER
    ticketDelete(
        ApplyView& view,
        AccountID const& account,
        uint256 const& ticketIndex,
        beast::Journal j);

protected:
    TER
    apply();

    explicit Transactor(ApplyContext& ctx);

    virtual void
    preCompute();

    virtual TER
    doApply() = 0;

    /** Compute the minimum fee required to process a transaction
        with a given baseFee based on the current server load.

        @param app The application hosting the server
        @param baseFee The base fee of a candidate transaction
            @see ripple::calculateBaseFee
        @param fees Fee settings from the current ledger
        @param flags Transaction processing fees
     */
    static BIXRPAmount
    minimumFee(
        Application& app,
        FeeUnit64 baseFee,
        Fees const& fees,
        ApplyFlags flags);

private:
    std::pair<TER, BIXRPAmount>
    reset(BIXRPAmount fee);

    TER
    consumeSeqProxy(SLE::pointer const& sleAccount);
    TER
    payFee();
    static NotTEC
    checkSingleSign(PreclaimContext const& ctx);
    static NotTEC
    checkMultiSign(PreclaimContext const& ctx);
};

/** Performs early sanity checks on the txid */
NotTEC
preflight0(PreflightContext const& ctx);

/** Performs early sanity checks on the account and fee fields */
NotTEC
preflight1(PreflightContext const& ctx);

/** Checks whether the signature appears valid */
NotTEC
preflight2(PreflightContext const& ctx);

}  // namespace ripple

#endif
