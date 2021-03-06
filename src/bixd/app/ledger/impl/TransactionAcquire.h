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

#ifndef BIXD_APP_LEDGER_TRANSACTIONACQUIRE_H_INCLUDED
#define BIXD_APP_LEDGER_TRANSACTIONACQUIRE_H_INCLUDED

#include <bixd/app/main/Application.h>
#include <bixd/overlay/PeerSet.h>
#include <bixd/shamap/SHAMap.h>

namespace bixd {

// VFALCO TODO rename to PeerTxRequest
// A transaction set we are trying to acquire
class TransactionAcquire final
    : public TimeoutCounter,
      public std::enable_shared_from_this<TransactionAcquire>,
      public CountedObject<TransactionAcquire>
{
public:
    using pointer = std::shared_ptr<TransactionAcquire>;

    TransactionAcquire(
        Application& app,
        uint256 const& hash,
        std::unique_ptr<PeerSet> peerSet);
    ~TransactionAcquire() = default;

    SHAMapAddNode
    takeNodes(
        const std::list<SHAMapNodeID>& IDs,
        const std::list<Blob>& data,
        std::shared_ptr<Peer> const&);

    void
    init(int startPeers);

    void
    stillNeed();

private:
    std::shared_ptr<SHAMap> mMap;
    bool mHaveRoot;
    std::unique_ptr<PeerSet> mPeerSet;

    void
    onTimer(bool progress, ScopedLockType& peerSetLock) override;

    void
    done();

    void
    addPeers(std::size_t limit);

    void
    trigger(std::shared_ptr<Peer> const&);
    std::weak_ptr<TimeoutCounter>
    pmDowncast() override;
};

}  // namespace bixd

#endif
