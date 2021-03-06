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

#ifndef BIXD_LEDGER_APPLYSTATETABLE_H_INCLUDED
#define BIXD_LEDGER_APPLYSTATETABLE_H_INCLUDED

#include <bixd/basics/BIXRPAmount.h>
#include <bixd/beast/utility/Journal.h>
#include <bixd/ledger/OpenView.h>
#include <bixd/ledger/RawView.h>
#include <bixd/ledger/ReadView.h>
#include <bixd/ledger/TxMeta.h>
#include <bixd/protocol/TER.h>
#include <memory>

namespace bixd {
namespace detail {

// Helper class that buffers modifications
class ApplyStateTable
{
public:
    using key_type = ReadView::key_type;

private:
    enum class Action {
        cache,
        erase,
        insert,
        modify,
    };

    using items_t = std::map<key_type, std::pair<Action, std::shared_ptr<SLE>>>;

    items_t items_;
    BIXRPAmount dropsDestroyed_{0};

public:
    ApplyStateTable() = default;
    ApplyStateTable(ApplyStateTable&&) = default;

    ApplyStateTable(ApplyStateTable const&) = delete;
    ApplyStateTable&
    operator=(ApplyStateTable&&) = delete;
    ApplyStateTable&
    operator=(ApplyStateTable const&) = delete;

    void
    apply(RawView& to) const;

    void
    apply(
        OpenView& to,
        STTx const& tx,
        TER ter,
        boost::optional<STAmount> const& deliver,
        beast::Journal j);

    bool
    exists(ReadView const& base, Keylet const& k) const;

    boost::optional<key_type>
    succ(
        ReadView const& base,
        key_type const& key,
        boost::optional<key_type> const& last) const;

    std::shared_ptr<SLE const>
    read(ReadView const& base, Keylet const& k) const;

    std::shared_ptr<SLE>
    peek(ReadView const& base, Keylet const& k);

    std::size_t
    size() const;

    void
    visit(
        ReadView const& base,
        std::function<void(
            uint256 const& key,
            bool isDelete,
            std::shared_ptr<SLE const> const& before,
            std::shared_ptr<SLE const> const& after)> const& func) const;

    void
    erase(ReadView const& base, std::shared_ptr<SLE> const& sle);

    void
    rawErase(ReadView const& base, std::shared_ptr<SLE> const& sle);

    void
    insert(ReadView const& base, std::shared_ptr<SLE> const& sle);

    void
    update(ReadView const& base, std::shared_ptr<SLE> const& sle);

    void
    replace(ReadView const& base, std::shared_ptr<SLE> const& sle);

    void
    destroyBIXRP(BIXRPAmount const& fee);

    // For debugging
    BIXRPAmount const&
    dropsDestroyed() const
    {
        return dropsDestroyed_;
    }

private:
    using Mods = hash_map<key_type, std::shared_ptr<SLE>>;

    static void
    threadItem(TxMeta& meta, std::shared_ptr<SLE> const& to);

    std::shared_ptr<SLE>
    getForMod(
        ReadView const& base,
        key_type const& key,
        Mods& mods,
        beast::Journal j);

    void
    threadTx(
        ReadView const& base,
        TxMeta& meta,
        AccountID const& to,
        Mods& mods,
        beast::Journal j);

    void
    threadOwners(
        ReadView const& base,
        TxMeta& meta,
        std::shared_ptr<SLE const> const& sle,
        Mods& mods,
        beast::Journal j);
};

}  // namespace detail
}  // namespace bixd

#endif
