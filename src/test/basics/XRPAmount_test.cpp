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

#include <bixd/basics/BIBIXRPAmount.h>
#include <bixd/beast/unit_test.h>

namespace bixd {

class BIXRPAmount_test : public beast::unit_test::suite
{
public:
    void
    testSigNum()
    {
        testcase("signum");

        for (auto i : {-1, 0, 1})
        {
            BIXRPAmount const x(i);

            if (i < 0)
                BEAST_EXPECT(x.signum() < 0);
            else if (i > 0)
                BEAST_EXPECT(x.signum() > 0);
            else
                BEAST_EXPECT(x.signum() == 0);
        }
    }

    void
    testBeastZero()
    {
        testcase("beast::Zero Comparisons");

        using beast::zero;

        for (auto i : {-1, 0, 1})
        {
            BIXRPAmount const x(i);

            BEAST_EXPECT((i == 0) == (x == zero));
            BEAST_EXPECT((i != 0) == (x != zero));
            BEAST_EXPECT((i < 0) == (x < zero));
            BEAST_EXPECT((i > 0) == (x > zero));
            BEAST_EXPECT((i <= 0) == (x <= zero));
            BEAST_EXPECT((i >= 0) == (x >= zero));

            BEAST_EXPECT((0 == i) == (zero == x));
            BEAST_EXPECT((0 != i) == (zero != x));
            BEAST_EXPECT((0 < i) == (zero < x));
            BEAST_EXPECT((0 > i) == (zero > x));
            BEAST_EXPECT((0 <= i) == (zero <= x));
            BEAST_EXPECT((0 >= i) == (zero >= x));
        }
    }

    void
    testComparisons()
    {
        testcase("BIXRP Comparisons");

        for (auto i : {-1, 0, 1})
        {
            BIXRPAmount const x(i);

            for (auto j : {-1, 0, 1})
            {
                BIXRPAmount const y(j);

                BEAST_EXPECT((i == j) == (x == y));
                BEAST_EXPECT((i != j) == (x != y));
                BEAST_EXPECT((i < j) == (x < y));
                BEAST_EXPECT((i > j) == (x > y));
                BEAST_EXPECT((i <= j) == (x <= y));
                BEAST_EXPECT((i >= j) == (x >= y));
            }
        }
    }

    void
    testAddSub()
    {
        testcase("Addition & Subtraction");

        for (auto i : {-1, 0, 1})
        {
            BIXRPAmount const x(i);

            for (auto j : {-1, 0, 1})
            {
                BIXRPAmount const y(j);

                BEAST_EXPECT(BIXRPAmount(i + j) == (x + y));
                BEAST_EXPECT(BIXRPAmount(i - j) == (x - y));

                BEAST_EXPECT((x + y) == (y + x));  // addition is commutative
            }
        }
    }

    void
    testDecimal()
    {
        // Tautology
        BEAST_EXPECT(DROPS_PER_BIXRP.decimalBIXRP() == 1);

        BIXRPAmount test{1};
        BEAST_EXPECT(test.decimalBIXRP() == 0.000001);

        test = -test;
        BEAST_EXPECT(test.decimalBIXRP() == -0.000001);

        test = 100'000'000;
        BEAST_EXPECT(test.decimalBIXRP() == 100);

        test = -test;
        BEAST_EXPECT(test.decimalBIXRP() == -100);
    }

    void
    testFunctions()
    {
        // Explicitly test every defined function for the BIXRPAmount class
        // since some of them are templated, but not used anywhere else.
        auto make = [&](auto x) -> BIXRPAmount { return BIXRPAmount{x}; };

        BIXRPAmount defaulted;
        (void)defaulted;
        BIXRPAmount test{0};
        BEAST_EXPECT(test.drops() == 0);

        test = make(beast::zero);
        BEAST_EXPECT(test.drops() == 0);

        test = beast::zero;
        BEAST_EXPECT(test.drops() == 0);

        test = make(100);
        BEAST_EXPECT(test.drops() == 100);

        test = make(100u);
        BEAST_EXPECT(test.drops() == 100);

        BIXRPAmount const targetSame{200u};
        test = make(targetSame);
        BEAST_EXPECT(test.drops() == 200);
        BEAST_EXPECT(test == targetSame);
        BEAST_EXPECT(test < BIXRPAmount{1000});
        BEAST_EXPECT(test > BIXRPAmount{100});

        test = std::int64_t(200);
        BEAST_EXPECT(test.drops() == 200);
        test = std::uint32_t(300);
        BEAST_EXPECT(test.drops() == 300);

        test = targetSame;
        BEAST_EXPECT(test.drops() == 200);
        auto testOther = test.dropsAs<std::uint32_t>();
        BEAST_EXPECT(testOther);
        BEAST_EXPECT(*testOther == 200);
        test = std::numeric_limits<std::uint64_t>::max();
        testOther = test.dropsAs<std::uint32_t>();
        BEAST_EXPECT(!testOther);
        test = -1;
        testOther = test.dropsAs<std::uint32_t>();
        BEAST_EXPECT(!testOther);

        test = targetSame * 2;
        BEAST_EXPECT(test.drops() == 400);
        test = 3 * targetSame;
        BEAST_EXPECT(test.drops() == 600);
        test = 20;
        BEAST_EXPECT(test.drops() == 20);

        test += targetSame;
        BEAST_EXPECT(test.drops() == 220);

        test -= targetSame;
        BEAST_EXPECT(test.drops() == 20);

        test *= 5;
        BEAST_EXPECT(test.drops() == 100);
        test = 50;
        BEAST_EXPECT(test.drops() == 50);
        test -= 39;
        BEAST_EXPECT(test.drops() == 11);

        // legal with signed
        test = -test;
        BEAST_EXPECT(test.drops() == -11);
        BEAST_EXPECT(test.signum() == -1);
        BEAST_EXPECT(to_string(test) == "-11");

        BEAST_EXPECT(test);
        test = 0;
        BEAST_EXPECT(!test);
        BEAST_EXPECT(test.signum() == 0);
        test = targetSame;
        BEAST_EXPECT(test.signum() == 1);
        BEAST_EXPECT(to_string(test) == "200");
    }

    void
    testMulRatio()
    {
        testcase("mulRatio");

        constexpr auto maxUInt32 = std::numeric_limits<std::uint32_t>::max();
        constexpr auto maxBIXRP =
            std::numeric_limits<BIXRPAmount::value_type>::max();
        constexpr auto minBIXRP =
            std::numeric_limits<BIXRPAmount::value_type>::min();

        {
            // multiply by a number that would overflow then divide by the same
            // number, and check we didn't lose any value
            BIXRPAmount big(maxBIXRP);
            BEAST_EXPECT(big == mulRatio(big, maxUInt32, maxUInt32, true));
            // rounding mode shouldn't matter as the result is exact
            BEAST_EXPECT(big == mulRatio(big, maxUInt32, maxUInt32, false));

            // multiply and divide by values that would overflow if done
            // naively, and check that it gives the correct answer
            big -= 0xf;  // Subtract a little so it's divisable by 4
            BEAST_EXPECT(
                mulRatio(big, 3, 4, false).value() == (big.value() / 4) * 3);
            BEAST_EXPECT(
                mulRatio(big, 3, 4, true).value() == (big.value() / 4) * 3);
            BEAST_EXPECT((big.value() * 3) / 4 != (big.value() / 4) * 3);
        }

        {
            // Similar test as above, but for negative values
            BIXRPAmount big(minBIXRP);
            BEAST_EXPECT(big == mulRatio(big, maxUInt32, maxUInt32, true));
            // rounding mode shouldn't matter as the result is exact
            BEAST_EXPECT(big == mulRatio(big, maxUInt32, maxUInt32, false));

            // multiply and divide by values that would overflow if done
            // naively, and check that it gives the correct answer
            BEAST_EXPECT(
                mulRatio(big, 3, 4, false).value() == (big.value() / 4) * 3);
            BEAST_EXPECT(
                mulRatio(big, 3, 4, true).value() == (big.value() / 4) * 3);
            BEAST_EXPECT((big.value() * 3) / 4 != (big.value() / 4) * 3);
        }

        {
            // small amounts
            BIXRPAmount tiny(1);
            // Round up should give the smallest allowable number
            BEAST_EXPECT(tiny == mulRatio(tiny, 1, maxUInt32, true));
            // rounding down should be zero
            BEAST_EXPECT(beast::zero == mulRatio(tiny, 1, maxUInt32, false));
            BEAST_EXPECT(
                beast::zero == mulRatio(tiny, maxUInt32 - 1, maxUInt32, false));

            // tiny negative numbers
            BIXRPAmount tinyNeg(-1);
            // Round up should give zero
            BEAST_EXPECT(beast::zero == mulRatio(tinyNeg, 1, maxUInt32, true));
            BEAST_EXPECT(
                beast::zero ==
                mulRatio(tinyNeg, maxUInt32 - 1, maxUInt32, true));
            // rounding down should be tiny
            BEAST_EXPECT(
                tinyNeg == mulRatio(tinyNeg, maxUInt32 - 1, maxUInt32, false));
        }

        {// rounding
         {BIXRPAmount one(1);
        auto const rup = mulRatio(one, maxUInt32 - 1, maxUInt32, true);
        auto const rdown = mulRatio(one, maxUInt32 - 1, maxUInt32, false);
        BEAST_EXPECT(rup.drops() - rdown.drops() == 1);
    }

    {
        BIXRPAmount big(maxBIXRP);
        auto const rup = mulRatio(big, maxUInt32 - 1, maxUInt32, true);
        auto const rdown = mulRatio(big, maxUInt32 - 1, maxUInt32, false);
        BEAST_EXPECT(rup.drops() - rdown.drops() == 1);
    }

    {
        BIXRPAmount negOne(-1);
        auto const rup = mulRatio(negOne, maxUInt32 - 1, maxUInt32, true);
        auto const rdown = mulRatio(negOne, maxUInt32 - 1, maxUInt32, false);
        BEAST_EXPECT(rup.drops() - rdown.drops() == 1);
    }
}

{
    // division by zero
    BIXRPAmount one(1);
    except([&] { mulRatio(one, 1, 0, true); });
}

{
    // overflow
    BIXRPAmount big(maxBIXRP);
    except([&] { mulRatio(big, 2, 1, true); });
}

{
    // underflow
    BIXRPAmount bigNegative(minBIXRP + 10);
    BEAST_EXPECT(mulRatio(bigNegative, 2, 1, true) == minBIXRP);
}
}  // namespace bixd

//--------------------------------------------------------------------------

void
run() override
{
    testSigNum();
    testBeastZero();
    testComparisons();
    testAddSub();
    testDecimal();
    testFunctions();
    testMulRatio();
}
}
;

BEAST_DEFINE_TESTSUITE(BIXRPAmount, protocol, bixd);

}  // bixd
