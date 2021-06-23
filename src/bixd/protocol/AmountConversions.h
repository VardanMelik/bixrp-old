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

#ifndef BIXD_PROTOCOL_AMOUNTCONVERSION_H_INCLUDED
#define BIXD_PROTOCOL_AMOUNTCONVERSION_H_INCLUDED

#include <bixd/basics/IOUAmount.h>
#include <bixd/basics/XRPAmount.h>
#include <bixd/protocol/STAmount.h>

namespace bixd {

inline STAmount
toSTAmount(IOUAmount const& iou, Issue const& iss)
{
    bool const isNeg = iou.signum() < 0;
    std::uint64_t const umant = isNeg ? -iou.mantissa() : iou.mantissa();
    return STAmount(
        iss,
        umant,
        iou.exponent(),
        /*native*/ false,
        isNeg,
        STAmount::unchecked());
}

inline STAmount
toSTAmount(IOUAmount const& iou)
{
    return toSTAmount(iou, noIssue());
}

inline STAmount
toSTAmount(BIXRPAmount const& bixrp)
{
    bool const isNeg = bixrp.signum() < 0;
    std::uint64_t const umant = isNeg ? -bixrp.drops() : bixrp.drops();
    return STAmount(umant, isNeg);
}

inline STAmount
toSTAmount(BIXRPAmount const& bixrp, Issue const& iss)
{
    assert(isBIXRP(iss.account) && isBIXRP(iss.currency));
    return toSTAmount(bixrp);
}

template <class T>
T
toAmount(STAmount const& amt) = delete;

template <>
inline STAmount
toAmount<STAmount>(STAmount const& amt)
{
    return amt;
}

template <>
inline IOUAmount
toAmount<IOUAmount>(STAmount const& amt)
{
    assert(amt.mantissa() < std::numeric_limits<std::int64_t>::max());
    bool const isNeg = amt.negative();
    std::int64_t const sMant =
        isNeg ? -std::int64_t(amt.mantissa()) : amt.mantissa();

    assert(!isBIXRP(amt));
    return IOUAmount(sMant, amt.exponent());
}

template <>
inline BIXRPAmount
toAmount<BIXRPAmount>(STAmount const& amt)
{
    assert(amt.mantissa() < std::numeric_limits<std::int64_t>::max());
    bool const isNeg = amt.negative();
    std::int64_t const sMant =
        isNeg ? -std::int64_t(amt.mantissa()) : amt.mantissa();

    assert(isBIXRP(amt));
    return BIXRPAmount(sMant);
}

template <class T>
T
toAmount(IOUAmount const& amt) = delete;

template <>
inline IOUAmount
toAmount<IOUAmount>(IOUAmount const& amt)
{
    return amt;
}

template <class T>
T
toAmount(BIXRPAmount const& amt) = delete;

template <>
inline BIXRPAmount
toAmount<BIXRPAmount>(BIXRPAmount const& amt)
{
    return amt;
}

}  // namespace bixd

#endif
