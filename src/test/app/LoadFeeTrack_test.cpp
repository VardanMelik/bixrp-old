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

#include <bixd/app/misc/LoadFeeTrack.h>
#include <bixd/beast/unit_test.h>
#include <bixd/core/Config.h>
#include <bixd/ledger/ReadView.h>

namespace bixd {

class LoadFeeTrack_test : public beast::unit_test::suite
{
public:
    void
    run() override
    {
        Config d;  // get a default configuration object
        LoadFeeTrack l;
        {
            Fees const fees = [&]() {
                Fees f;
                f.base = d.FEE_DEFAULT;
                f.units = d.TRANSACTION_FEE_BASE;
                f.reserve = 200 * DROPS_PER_BIXRP;
                f.increment = 50 * DROPS_PER_BIXRP;
                return f;
            }();

            BEAST_EXPECT(
                scaleFeeLoad(FeeUnit64{0}, l, fees, false) == BIXRPAmount{0});
            BEAST_EXPECT(
                scaleFeeLoad(FeeUnit64{10000}, l, fees, false) ==
                BIXRPAmount{10000});
            BEAST_EXPECT(
                scaleFeeLoad(FeeUnit64{1}, l, fees, false) == BIXRPAmount{1});
        }
        {
            Fees const fees = [&]() {
                Fees f;
                f.base = d.FEE_DEFAULT * 10;
                f.units = d.TRANSACTION_FEE_BASE;
                f.reserve = 200 * DROPS_PER_BIXRP;
                f.increment = 50 * DROPS_PER_BIXRP;
                return f;
            }();

            BEAST_EXPECT(
                scaleFeeLoad(FeeUnit64{0}, l, fees, false) == BIXRPAmount{0});
            BEAST_EXPECT(
                scaleFeeLoad(FeeUnit64{10000}, l, fees, false) ==
                BIXRPAmount{100000});
            BEAST_EXPECT(
                scaleFeeLoad(FeeUnit64{1}, l, fees, false) == BIXRPAmount{10});
        }
        {
            Fees const fees = [&]() {
                Fees f;
                f.base = d.FEE_DEFAULT;
                f.units = d.TRANSACTION_FEE_BASE * 10;
                f.reserve = 200 * DROPS_PER_BIXRP;
                f.increment = 50 * DROPS_PER_BIXRP;
                return f;
            }();

            BEAST_EXPECT(
                scaleFeeLoad(FeeUnit64{0}, l, fees, false) == BIXRPAmount{0});
            BEAST_EXPECT(
                scaleFeeLoad(FeeUnit64{10000}, l, fees, false) ==
                BIXRPAmount{1000});
            BEAST_EXPECT(
                scaleFeeLoad(FeeUnit64{1}, l, fees, false) == BIXRPAmount{0});
        }
    }
};

BEAST_DEFINE_TESTSUITE(LoadFeeTrack, bixd_core, bixd);

}  // namespace bixd
