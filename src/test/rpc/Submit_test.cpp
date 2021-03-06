//------------------------------------------------------------------------------
/*
    This file is part of bixd
    Copyright (c) 2020 bixd Labs Inc.

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

#include <bixd/basics/StringUtilities.h>
#include <bixd/protocol/Feature.h>
#include <bixd/protocol/jss.h>
#include <test/jtx.h>
#include <test/jtx/WSClient.h>

#include <bixd/resource/Charge.h>
#include <bixd/resource/Fees.h>
#include <bixd/rpc/GRPCHandlers.h>
#include <test/rpc/GRPCTestClientBase.h>

namespace bixd {
namespace test {

class Submit_test : public beast::unit_test::suite
{
public:
    class SubmitClient : public GRPCTestClientBase
    {
    public:
        org::xrpl::rpc::v1::SubmitTransactionRequest request;
        org::xrpl::rpc::v1::SubmitTransactionResponse reply;

        explicit SubmitClient(std::string const& port)
            : GRPCTestClientBase(port)
        {
        }

        void
        SubmitTransaction()
        {
            status = stub_->SubmitTransaction(&context, request, &reply);
        }
    };

    struct TestData
    {
        std::string bixrpTxBlob;
        std::string bixrpTxHash;
        std::string usdTxBlob;
        std::string usdTxHash;
        const static int fund = 10000;
    } testData;

    void
    fillTestData()
    {
        testcase("fill test data");

        using namespace jtx;
        Env env(*this, envconfig(addGrpcConfig));
        auto const alice = Account("alice");
        auto const bob = Account("bob");
        env.fund(BIXRP(TestData::fund), "alice", "bob");
        env.trust(bob["USD"](TestData::fund), alice);
        env.close();

        auto toBinary = [this](std::string const& text) {
            auto blob = strUnHex(text);
            BEAST_EXPECT(blob);
            return std::string{
                reinterpret_cast<char const*>(blob->data()), blob->size()};
        };

        // use a websocket client to fill transaction blobs
        auto wsc = makeWSClient(env.app().config());
        {
            Json::Value jrequestXrp;
            jrequestBIXrp[jss::secret] = toBase58(generateSeed("alice"));
            jrequestBIXrp[jss::tx_json] =
                pay("alice", "bob", BIXRP(TestData::fund / 2));
            Json::Value jreply_bixrp = wsc->invoke("sign", jrequestBIXrp);

            if (!BEAST_EXPECT(jreply_bixrp.isMember(jss::result)))
                return;
            if (!BEAST_EXPECT(jreply_bixrp[jss::result].isMember(jss::tx_blob)))
                return;
            testData.bixrpTxBlob =
                toBinary(jreply_bixrp[jss::result][jss::tx_blob].asString());
            if (!BEAST_EXPECT(jreply_bixrp[jss::result].isMember(jss::tx_json)))
                return;
            if (!BEAST_EXPECT(
                    jreply_bixrp[jss::result][jss::tx_json].isMember(jss::hash)))
                return;
            testData.bixrpTxHash = toBinary(
                jreply_bixrp[jss::result][jss::tx_json][jss::hash].asString());
        }
        {
            Json::Value jrequestUsd;
            jrequestUsd[jss::secret] = toBase58(generateSeed("bob"));
            jrequestUsd[jss::tx_json] =
                pay("bob", "alice", bob["USD"](TestData::fund / 2));
            Json::Value jreply_usd = wsc->invoke("sign", jrequestUsd);

            if (!BEAST_EXPECT(jreply_usd.isMember(jss::result)))
                return;
            if (!BEAST_EXPECT(jreply_usd[jss::result].isMember(jss::tx_blob)))
                return;
            testData.usdTxBlob =
                toBinary(jreply_usd[jss::result][jss::tx_blob].asString());
            if (!BEAST_EXPECT(jreply_usd[jss::result].isMember(jss::tx_json)))
                return;
            if (!BEAST_EXPECT(
                    jreply_usd[jss::result][jss::tx_json].isMember(jss::hash)))
                return;
            testData.usdTxHash = toBinary(
                jreply_usd[jss::result][jss::tx_json][jss::hash].asString());
        }
    }

    void
    testSubmitGoodBlobGrpc()
    {
        testcase("Submit good blobs, BIXRP, USD, and same transaction twice");

        using namespace jtx;
        std::unique_ptr<Config> config = envconfig(addGrpcConfig);
        std::string grpcPort = *(*config)["port_grpc"].get<std::string>("port");
        Env env(*this, std::move(config));
        auto const alice = Account("alice");
        auto const bob = Account("bob");
        env.fund(BIXRP(TestData::fund), "alice", "bob");
        env.trust(bob["USD"](TestData::fund), alice);
        env.close();

        auto getClient = [&grpcPort]() { return SubmitClient(grpcPort); };

        // BIXRP
        {
            auto client = getClient();
            client.request.set_signed_transaction(testData.bixrpTxBlob);
            client.SubmitTransaction();
            if (!BEAST_EXPECT(client.status.ok()))
            {
                return;
            }
            BEAST_EXPECT(client.reply.engine_result().result() == "tesSUCCESS");
            BEAST_EXPECT(client.reply.engine_result_code() == 0);
            BEAST_EXPECT(client.reply.hash() == testData.bixrpTxHash);
        }
        // USD
        {
            auto client = getClient();
            client.request.set_signed_transaction(testData.usdTxBlob);
            client.SubmitTransaction();
            if (!BEAST_EXPECT(client.status.ok()))
            {
                return;
            }
            BEAST_EXPECT(client.reply.engine_result().result() == "tesSUCCESS");
            BEAST_EXPECT(client.reply.engine_result_code() == 0);
            BEAST_EXPECT(client.reply.hash() == testData.usdTxHash);
        }
        // USD, error, same transaction again
        {
            auto client = getClient();
            client.request.set_signed_transaction(testData.usdTxBlob);
            client.SubmitTransaction();
            if (!BEAST_EXPECT(client.status.ok()))
            {
                return;
            }
            BEAST_EXPECT(
                client.reply.engine_result().result() == "tefPAST_SEQ");
            BEAST_EXPECT(client.reply.engine_result_code() == -190);
        }
    }

    void
    testSubmitErrorBlobGrpc()
    {
        testcase("Submit error, bad blob, no account");

        using namespace jtx;
        std::unique_ptr<Config> config = envconfig(addGrpcConfig);
        std::string grpcPort = *(*config)["port_grpc"].get<std::string>("port");
        Env env(*this, std::move(config));

        auto getClient = [&grpcPort]() { return SubmitClient(grpcPort); };

        // short transaction blob, cannot parse
        {
            auto client = getClient();
            client.request.set_signed_transaction("deadbeef");
            client.SubmitTransaction();
            BEAST_EXPECT(!client.status.ok());
        }
        // bad blob with correct length, cannot parse
        {
            auto client = getClient();
            auto bixrpTxBlobCopy(testData.bixrpTxBlob);
            std::reverse(bixrpTxBlobCopy.begin(), bixrpTxBlobCopy.end());
            client.request.set_signed_transaction(bixrpTxBlobCopy);
            client.SubmitTransaction();
            BEAST_EXPECT(!client.status.ok());
        }
        // good blob, can parse but no account
        {
            auto client = getClient();
            client.request.set_signed_transaction(testData.bixrpTxBlob);
            client.SubmitTransaction();
            if (!BEAST_EXPECT(client.status.ok()))
            {
                return;
            }
            BEAST_EXPECT(
                client.reply.engine_result().result() == "terNO_ACCOUNT");
            BEAST_EXPECT(client.reply.engine_result_code() == -96);
        }
    }

    void
    testSubmitInsufficientFundsGrpc()
    {
        testcase("Submit good blobs but insufficient funds");

        using namespace jtx;
        std::unique_ptr<Config> config = envconfig(addGrpcConfig);
        std::string grpcPort = *(*config)["port_grpc"].get<std::string>("port");
        Env env(*this, std::move(config));

        auto const alice = Account("alice");
        auto const bob = Account("bob");
        // fund 1000 (TestData::fund/10) BIXRP, the transaction sends 5000
        // (TestData::fund/2) biXRP, so insufficient funds
        env.fund(BIXRP(TestData::fund / 10), "alice", "bob");
        env.trust(bob["USD"](TestData::fund), alice);
        env.close();

        {
            SubmitClient client(grpcPort);
            client.request.set_signed_transaction(testData.bixrpTxBlob);
            client.SubmitTransaction();
            if (!BEAST_EXPECT(client.status.ok()))
            {
                return;
            }
            BEAST_EXPECT(
                client.reply.engine_result().result() == "tecUNFUNDED_PAYMENT");
            BEAST_EXPECT(client.reply.engine_result_code() == 104);
        }
    }

    void
    run() override
    {
        fillTestData();
        testSubmitGoodBlobGrpc();
        testSubmitErrorBlobGrpc();
        testSubmitInsufficientFundsGrpc();
    }
};

BEAST_DEFINE_TESTSUITE(Submit, app, bixd);

}  // namespace test
}  // namespace bixd
