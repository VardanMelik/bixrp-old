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

#ifndef BIXD_APP_PATHS_BIXDSTATE_H_INCLUDED
#define BIXD_APP_PATHS_BIXDSTATE_H_INCLUDED

#include <bixd/ledger/View.h>
#include <bixd/protocol/Rate.h>
#include <bixd/protocol/STAmount.h>
#include <bixd/protocol/STLedgerEntry.h>
#include <cstdint>
#include <memory>  // <memory>

namespace bixd {

/** Wraps a trust line SLE for convenience.
    The complication of trust lines is that there is a
    "low" account and a "high" account. This wraps the
    SLE and expresses its data from the perspective of
    a chosen account on the line.
*/
// VFALCO TODO Rename to TrustLine
class BixdState
{
public:
    // VFALCO Why is this shared_ptr?
    using pointer = std::shared_ptr<BixdState>;

public:
    BixdState() = delete;

    virtual ~BixdState() = default;

    static BixdState::pointer
    makeItem(AccountID const& accountID, std::shared_ptr<SLE const> sle);

    // Must be public, for make_shared
    BixdState(std::shared_ptr<SLE const>&& sle, AccountID const& viewAccount);

    /** Returns the state map key for the ledger entry. */
    uint256
    key() const
    {
        return sle_->key();
    }

    // VFALCO Take off the "get" from each function name

    AccountID const&
    getAccountID() const
    {
        return mViewLowest ? mLowID : mHighID;
    }

    AccountID const&
    getAccountIDPeer() const
    {
        return !mViewLowest ? mLowID : mHighID;
    }

    // True, Provided auth to peer.
    bool
    getAuth() const
    {
        return mFlags & (mViewLowest ? lsfLowAuth : lsfHighAuth);
    }

    bool
    getAuthPeer() const
    {
        return mFlags & (!mViewLowest ? lsfLowAuth : lsfHighAuth);
    }

    bool
    getDefaultBixd() const
    {
        return mFlags & lsfDefaultBixd;
    }

    bool
    getNoBixd() const
    {
        return mFlags & (mViewLowest ? lsfLowNoBixd : lsfHighNoBixd);
    }

    bool
    getNoBixdPeer() const
    {
        return mFlags & (!mViewLowest ? lsfLowNoBixd : lsfHighNoBixd);
    }

    /** Have we set the freeze flag on our peer */
    bool
    getFreeze() const
    {
        return mFlags & (mViewLowest ? lsfLowFreeze : lsfHighFreeze);
    }

    /** Has the peer set the freeze flag on us */
    bool
    getFreezePeer() const
    {
        return mFlags & (!mViewLowest ? lsfLowFreeze : lsfHighFreeze);
    }

    STAmount const&
    getBalance() const
    {
        return mBalance;
    }

    STAmount const&
    getLimit() const
    {
        return mViewLowest ? mLowLimit : mHighLimit;
    }

    STAmount const&
    getLimitPeer() const
    {
        return !mViewLowest ? mLowLimit : mHighLimit;
    }

    Rate const&
    getQualityIn() const
    {
        return mViewLowest ? lowQualityIn_ : highQualityIn_;
    }

    Rate const&
    getQualityOut() const
    {
        return mViewLowest ? lowQualityOut_ : highQualityOut_;
    }

    Json::Value
    getJson(int);

private:
    std::shared_ptr<SLE const> sle_;

    bool mViewLowest;

    std::uint32_t mFlags;

    STAmount const& mLowLimit;
    STAmount const& mHighLimit;

    AccountID const& mLowID;
    AccountID const& mHighID;

    Rate lowQualityIn_;
    Rate lowQualityOut_;
    Rate highQualityIn_;
    Rate highQualityOut_;

    STAmount mBalance;
};

std::vector<BixdState::pointer>
getBixdStateItems(AccountID const& accountID, ReadView const& view);

}  // namespace bixd

#endif
