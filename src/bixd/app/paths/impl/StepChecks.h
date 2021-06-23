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

#ifndef BIXD_APP_PATHS_IMPL_STEP_CHECKS_H_INCLUDED
#define BIXD_APP_PATHS_IMPL_STEP_CHECKS_H_INCLUDED

#include <bixd/basics/Log.h>
#include <bixd/beast/utility/Journal.h>
#include <bixd/ledger/ReadView.h>
#include <bixd/protocol/AccountID.h>
#include <bixd/protocol/UintTypes.h>

namespace bixd {

inline TER
checkFreeze(
    ReadView const& view,
    AccountID const& src,
    AccountID const& dst,
    Currency const& currency)
{
    assert(src != dst);

    // check freeze
    if (auto sle = view.read(keylet::account(dst)))
    {
        if (sle->isFlag(lsfGlobalFreeze))
        {
            return terNO_LINE;
        }
    }

    if (auto sle = view.read(keylet::line(src, dst, currency)))
    {
        if (sle->isFlag((dst > src) ? lsfHighFreeze : lsfLowFreeze))
        {
            return terNO_LINE;
        }
    }

    return tesSUCCESS;
}

inline TER
checkNoRipple(
    ReadView const& view,
    AccountID const& prev,
    AccountID const& cur,
    // This is the account whose constraints we are checking
    AccountID const& next,
    Currency const& currency,
    beast::Journal j)
{
    // fetch the bixd lines into and out of this node
    auto sleIn = view.read(keylet::line(prev, cur, currency));
    auto sleOut = view.read(keylet::line(cur, next, currency));

    if (!sleIn || !sleOut)
        return terNO_LINE;

    if ((*sleIn)[sfFlags] & ((cur > prev) ? lsfHighNoRipple : lsfLowNoRipple) &&
        (*sleOut)[sfFlags] & ((cur > next) ? lsfHighNoRipple : lsfLowNoRipple))
    {
        JLOG(j.info()) << "Path violates noRipple constraint between " << prev
                       << ", " << cur << " and " << next;

        return terNO_BIXD;
    }
    return tesSUCCESS;
}

}  // namespace bixd

#endif
