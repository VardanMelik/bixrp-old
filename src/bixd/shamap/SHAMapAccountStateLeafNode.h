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

#ifndef BIXD_SHAMAP_SHAMAPACCOUNTSTATELEAFNODE_H_INCLUDED
#define BIXD_SHAMAP_SHAMAPACCOUNTSTATELEAFNODE_H_INCLUDED

#include <bixd/basics/CountedObject.h>
#include <bixd/protocol/HashPrefix.h>
#include <bixd/protocol/digest.h>
#include <bixd/shamap/SHAMapItem.h>
#include <bixd/shamap/SHAMapLeafNode.h>
#include <bixd/shamap/SHAMapNodeID.h>

namespace bixd {

/** A leaf node for a state object. */
class SHAMapAccountStateLeafNode final
    : public SHAMapLeafNode,
      public CountedObject<SHAMapAccountStateLeafNode>
{
public:
    SHAMapAccountStateLeafNode(
        std::shared_ptr<SHAMapItem const> item,
        std::uint32_t cowid)
        : SHAMapLeafNode(std::move(item), cowid)
    {
        updateHash();
    }

    SHAMapAccountStateLeafNode(
        std::shared_ptr<SHAMapItem const> item,
        std::uint32_t cowid,
        SHAMapHash const& hash)
        : SHAMapLeafNode(std::move(item), cowid, hash)
    {
    }

    std::shared_ptr<SHAMapTreeNode>
    clone(std::uint32_t cowid) const final override
    {
        return std::make_shared<SHAMapAccountStateLeafNode>(
            item_, cowid, hash_);
    }

    SHAMapNodeType
    getType() const final override
    {
        return SHAMapNodeType::tnACCOUNT_STATE;
    }

    void
    updateHash() final override
    {
        hash_ = SHAMapHash{sha512Half(
            HashPrefix::leafNode, makeSlice(item_->peekData()), item_->key())};
    }

    void
    serializeForWire(Serializer& s) const final override
    {
        s.addRaw(item_->peekData());
        s.addBitString(item_->key());
        s.add8(wireTypeAccountState);
    }

    void
    serializeWithPrefix(Serializer& s) const final override
    {
        s.add32(HashPrefix::leafNode);
        s.addRaw(item_->peekData());
        s.addBitString(item_->key());
    }
};

}  // namespace bixd

#endif
