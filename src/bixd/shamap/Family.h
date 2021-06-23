//------------------------------------------------------------------------------
/*
    This file is part of bixd
    Copyright (c) 2015 bixd Labs Inc.

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

#ifndef BIXD_SHAMAP_FAMILY_H_INCLUDED
#define BIXD_SHAMAP_FAMILY_H_INCLUDED

#include <bixd/basics/Log.h>
#include <bixd/beast/utility/Journal.h>
#include <bixd/nodestore/Database.h>
#include <bixd/shamap/FullBelowCache.h>
#include <bixd/shamap/TreeNodeCache.h>
#include <cstdint>

namespace bixd {

class Family
{
public:
    Family(Family const&) = delete;
    Family(Family&&) = delete;

    Family&
    operator=(Family const&) = delete;

    Family&
    operator=(Family&&) = delete;

    explicit Family() = default;
    virtual ~Family() = default;

    virtual NodeStore::Database&
    db() = 0;

    virtual NodeStore::Database const&
    db() const = 0;

    virtual beast::Journal const&
    journal() = 0;

    /** Return a pointer to the Family Full Below Cache

        @param ledgerSeq ledger sequence determines a corresponding shard cache
        @note ledgerSeq is used by ShardFamily and ignored by NodeFamily
    */
    virtual std::shared_ptr<FullBelowCache>
    getFullBelowCache(std::uint32_t ledgerSeq) = 0;

    /** Return a pointer to the Family Tree Node Cache

        @param ledgerSeq ledger sequence determines a corresponding shard cache
        @note ledgerSeq is used by ShardFamily and ignored by NodeFamily
    */
    virtual std::shared_ptr<TreeNodeCache>
    getTreeNodeCache(std::uint32_t ledgerSeq) = 0;

    virtual void
    sweep() = 0;

    virtual bool
    isShardBacked() const = 0;

    virtual void
    missingNode(std::uint32_t refNum) = 0;

    virtual void
    missingNode(uint256 const& refHash, std::uint32_t refNum) = 0;

    virtual void
    reset() = 0;
};

}  // namespace bixd

#endif
