//------------------------------------------------------------------------------
/*
    This file is part of bixd
    Copyright (c) 2012-2014 bixd Labs Inc.

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

#include <bixd/app/main/Application.h>
#include <bixd/app/paths/BixdState.h>
#include <bixd/ledger/ReadView.h>
#include <bixd/net/RPCErr.h>
#include <bixd/protocol/ErrorCodes.h>
#include <bixd/protocol/jss.h>
#include <bixd/rpc/Context.h>
#include <bixd/rpc/impl/RPCHelpers.h>

namespace bixd {

Json::Value
doAccountCurrencies(RPC::JsonContext& context)
{
    auto& params = context.params;

    // Get the current ledger
    std::shared_ptr<ReadView const> ledger;
    auto result = RPC::lookupLedger(ledger, context);
    if (!ledger)
        return result;

    if (!(params.isMember(jss::account) || params.isMember(jss::ident)))
        return RPC::missing_field_error(jss::account);

    std::string const strIdent(
        params.isMember(jss::account) ? params[jss::account].asString()
                                      : params[jss::ident].asString());

    bool const bStrict =
        params.isMember(jss::strict) && params[jss::strict].asBool();

    // Get info on account.
    AccountID accountID;  // out param
    if (auto jvAccepted = RPC::accountFromString(accountID, strIdent, bStrict))
        return jvAccepted;

    if (!ledger->exists(keylet::account(accountID)))
        return rpcError(rpcACT_NOT_FOUND);

    std::set<Currency> send, receive;
    for (auto const& item : getBixdStateItems(accountID, *ledger))
    {
        auto const rspEntry = item.get();

        STAmount const& saBalance = rspEntry->getBalance();

        if (saBalance < rspEntry->getLimit())
            receive.insert(saBalance.getCurrency());
        if ((-saBalance) < rspEntry->getLimitPeer())
            send.insert(saBalance.getCurrency());
    }

    send.erase(badCurrency());
    receive.erase(badCurrency());

    Json::Value& sendCurrencies =
        (result[jss::send_currencies] = Json::arrayValue);
    for (auto const& c : send)
        sendCurrencies.append(to_string(c));

    Json::Value& recvCurrencies =
        (result[jss::receive_currencies] = Json::arrayValue);
    for (auto const& c : receive)
        recvCurrencies.append(to_string(c));

    return result;
}

}  // namespace bixd
