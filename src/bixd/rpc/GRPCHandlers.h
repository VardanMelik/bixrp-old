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

#ifndef BIXD_RPC_GRPCHANDLER_H_INCLUDED
#define BIXD_RPC_GRPCHANDLER_H_INCLUDED

#include <bixd/rpc/Context.h>
#include <grpcpp/grpcpp.h>
#include <org/xrpl/rpc/v1/xrp_ledger.pb.h>

namespace bixd {

/*
 * These handlers are for gRPC. They each take in a protobuf message that is
 * nested inside RPC::GRPCContext<T>, where T is the request type
 * The return value is the response type, as well as a status
 * If the status is not Status::OK (meaning an error occurred), then only
 * the status will be sent to the client, and the response will be ommitted
 */

std::pair<org::xrpl::rpc::v1::GetAccountInfoResponse, grpc::Status>
doAccountInfoGrpc(
    RPC::GRPCContext<org::xrpl::rpc::v1::GetAccountInfoRequest>& context);

std::pair<org::xrpl::rpc::v1::GetFeeResponse, grpc::Status>
doFeeGrpc(RPC::GRPCContext<org::xrpl::rpc::v1::GetFeeRequest>& context);

std::pair<org::xrpl::rpc::v1::SubmitTransactionResponse, grpc::Status>
doSubmitGrpc(
    RPC::GRPCContext<org::xrpl::rpc::v1::SubmitTransactionRequest>& context);

// NOTE, this only supports Payment transactions at this time
std::pair<org::xrpl::rpc::v1::GetTransactionResponse, grpc::Status>
doTxGrpc(RPC::GRPCContext<org::xrpl::rpc::v1::GetTransactionRequest>& context);

std::pair<
    org::xrpl::rpc::v1::GetAccountTransactionHistoryResponse,
    grpc::Status>
doAccountTxGrpc(
    RPC::GRPCContext<org::xrpl::rpc::v1::GetAccountTransactionHistoryRequest>&
        context);

std::pair<org::xrpl::rpc::v1::GetLedgerResponse, grpc::Status>
doLedgerGrpc(RPC::GRPCContext<org::xrpl::rpc::v1::GetLedgerRequest>& context);

std::pair<org::xrpl::rpc::v1::GetLedgerEntryResponse, grpc::Status>
doLedgerEntryGrpc(
    RPC::GRPCContext<org::xrpl::rpc::v1::GetLedgerEntryRequest>& context);

std::pair<org::xrpl::rpc::v1::GetLedgerDataResponse, grpc::Status>
doLedgerDataGrpc(
    RPC::GRPCContext<org::xrpl::rpc::v1::GetLedgerDataRequest>& context);

std::pair<org::xrpl::rpc::v1::GetLedgerDiffResponse, grpc::Status>
doLedgerDiffGrpc(
    RPC::GRPCContext<org::xrpl::rpc::v1::GetLedgerDiffRequest>& context);

}  // namespace bixd

#endif
