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

#include <bixd/app/paths/Credit.h>
#include <bixd/app/paths/Flow.h>
#include <bixd/app/paths/impl/AmountSpec.h>
#include <bixd/app/paths/impl/Steps.h>
#include <bixd/app/paths/impl/StrandFlow.h>
#include <bixd/basics/IOUAmount.h>
#include <bixd/basics/Log.h>
#include <bixd/basics/BIXRPAmount.h>

#include <boost/container/flat_set.hpp>

#include <numeric>
#include <sstream>

namespace bixd {

template <class FlowResult>
static auto
finishFlow(
    PaymentSandbox& sb,
    Issue const& srcIssue,
    Issue const& dstIssue,
    FlowResult&& f)
{
    path::BixdCalc::Output result;
    if (f.ter == tesSUCCESS)
        f.sandbox->apply(sb);
    else
        result.removableOffers = std::move(f.removableOffers);

    result.setResult(f.ter);
    result.actualAmountIn = toSTAmount(f.in, srcIssue);
    result.actualAmountOut = toSTAmount(f.out, dstIssue);

    return result;
};

path::BixdCalc::Output
flow(
    PaymentSandbox& sb,
    STAmount const& deliver,
    AccountID const& src,
    AccountID const& dst,
    STPathSet const& paths,
    bool defaultPaths,
    bool partialPayment,
    bool ownerPaysTransferFee,
    bool offerCrossing,
    boost::optional<Quality> const& limitQuality,
    boost::optional<STAmount> const& sendMax,
    beast::Journal j,
    path::detail::FlowDebugInfo* flowDebugInfo)
{
    Issue const srcIssue = [&] {
        if (sendMax)
            return sendMax->issue();
        if (!isBIXRP(deliver.issue().currency))
            return Issue(deliver.issue().currency, src);
        return bixrpIssue();
    }();

    Issue const dstIssue = deliver.issue();

    boost::optional<Issue> sendMaxIssue;
    if (sendMax)
        sendMaxIssue = sendMax->issue();

    // convert the paths to a collection of strands. Each strand is the
    // collection of account->account steps and book steps that may be used in
    // this payment.
    auto [toStrandsTer, strands] = toStrands(
        sb,
        src,
        dst,
        dstIssue,
        limitQuality,
        sendMaxIssue,
        paths,
        defaultPaths,
        ownerPaysTransferFee,
        offerCrossing,
        j);

    if (toStrandsTer != tesSUCCESS)
    {
        path::BixdCalc::Output result;
        result.setResult(toStrandsTer);
        return result;
    }

    if (j.trace())
    {
        j.trace() << "\nsrc: " << src << "\ndst: " << dst
                  << "\nsrcIssue: " << srcIssue << "\ndstIssue: " << dstIssue;
        j.trace() << "\nNumStrands: " << strands.size();
        for (auto const& curStrand : strands)
        {
            j.trace() << "NumSteps: " << curStrand.size();
            for (auto const& step : curStrand)
            {
                j.trace() << '\n' << *step << '\n';
            }
        }
    }

    const bool srcIsBIXRP = isBIXRP(srcIssue.currency);
    const bool dstIsBIXRP = isBIXRP(dstIssue.currency);

    auto const asDeliver = toAmountSpec(deliver);

    // The src account may send either bixrp or iou. The dst account may receive
    // either bixrp or iou. Since BIXRP and IOU amounts are represented by different
    // types, use templates to tell `flow` about the amount types.
    if (srcIsBIXRP && dstIsBIXRP)
    {
        return finishFlow(
            sb,
            srcIssue,
            dstIssue,
            flow<BIXRPAmount, BIXRPAmount>(
                sb,
                strands,
                asDeliver.bixrp,
                partialPayment,
                offerCrossing,
                limitQuality,
                sendMax,
                j,
                flowDebugInfo));
    }

    if (srcIsBIXRP && !dstIsBIXRP)
    {
        return finishFlow(
            sb,
            srcIssue,
            dstIssue,
            flow<BIXRPAmount, IOUAmount>(
                sb,
                strands,
                asDeliver.iou,
                partialPayment,
                offerCrossing,
                limitQuality,
                sendMax,
                j,
                flowDebugInfo));
    }

    if (!srcIsBIXRP && dstIsBIXRP)
    {
        return finishFlow(
            sb,
            srcIssue,
            dstIssue,
            flow<IOUAmount, BIXRPAmount>(
                sb,
                strands,
                asDeliver.bixrp,
                partialPayment,
                offerCrossing,
                limitQuality,
                sendMax,
                j,
                flowDebugInfo));
    }

    assert(!srcIsBIXRP && !dstIsBIXRP);
    return finishFlow(
        sb,
        srcIssue,
        dstIssue,
        flow<IOUAmount, IOUAmount>(
            sb,
            strands,
            asDeliver.iou,
            partialPayment,
            offerCrossing,
            limitQuality,
            sendMax,
            j,
            flowDebugInfo));
}

}  // namespace bixd
