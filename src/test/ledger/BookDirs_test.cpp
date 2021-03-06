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

#include <bixd/ledger/BookDirs.h>
#include <bixd/protocol/Feature.h>
#include <test/jtx.h>

namespace bixd {
namespace test {

struct BookDirs_test : public beast::unit_test::suite
{
    void
    test_bookdir(FeatureBitset features)
    {
        using namespace jtx;
        Env env(*this, features);
        auto gw = Account("gw");
        auto USD = gw["USD"];
        env.fund(BIXRP(1000000), "alice", "bob", "gw");

        {
            Book book(bixrpIssue(), USD.issue());
            {
                auto d = BookDirs(*env.current(), book);
                BEAST_EXPECT(std::begin(d) == std::end(d));
                BEAST_EXPECT(std::distance(d.begin(), d.end()) == 0);
            }
            {
                auto d = BookDirs(*env.current(), reversed(book));
                BEAST_EXPECT(std::distance(d.begin(), d.end()) == 0);
            }
        }

        {
            env(offer("alice", Account("alice")["USD"](50), BIXRP(10)));
            auto d = BookDirs(
                *env.current(),
                Book(Account("alice")["USD"].issue(), bixrpIssue()));
            BEAST_EXPECT(std::distance(d.begin(), d.end()) == 1);
        }

        {
            env(offer("alice", gw["CNY"](50), BIXRP(10)));
            auto d =
                BookDirs(*env.current(), Book(gw["CNY"].issue(), bixrpIssue()));
            BEAST_EXPECT(std::distance(d.begin(), d.end()) == 1);
        }

        {
            env.trust(Account("bob")["CNY"](10), "alice");
            env(pay("bob", "alice", Account("bob")["CNY"](10)));
            env(offer("alice", USD(50), Account("bob")["CNY"](10)));
            auto d = BookDirs(
                *env.current(),
                Book(USD.issue(), Account("bob")["CNY"].issue()));
            BEAST_EXPECT(std::distance(d.begin(), d.end()) == 1);
        }

        {
            auto AUD = gw["AUD"];
            for (auto i = 1, j = 3; i <= 3; ++i, --j)
                for (auto k = 0; k < 80; ++k)
                    env(offer("alice", AUD(i), BIXRP(j)));

            auto d = BookDirs(*env.current(), Book(AUD.issue(), bixrpIssue()));
            BEAST_EXPECT(std::distance(d.begin(), d.end()) == 240);
            auto i = 1, j = 3, k = 0;
            for (auto const& e : d)
            {
                BEAST_EXPECT(e->getFieldAmount(sfTakerPays) == AUD(i));
                BEAST_EXPECT(e->getFieldAmount(sfTakerGets) == BIXRP(j));
                if (++k % 80 == 0)
                {
                    ++i;
                    --j;
                }
            }
        }
    }

    void
    run() override
    {
        using namespace jtx;
        auto const sa = supported_amendments();
        test_bookdir(sa - featureFlowCross);
        test_bookdir(sa);
    }
};

BEAST_DEFINE_TESTSUITE(BookDirs, ledger, bixd);

}  // namespace test
}  // namespace bixd
