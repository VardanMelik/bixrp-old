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

#include <bixd/ledger/ReadView.h>
#include <bixd/protocol/AmountConversions.h>
#include <bixd/protocol/Indexes.h>
#include <bixd/protocol/STAmount.h>

namespace bixd {

STAmount
creditLimit(
    ReadView const& view,
    AccountID const& account,
    AccountID const& issuer,
    Currency const& currency)
{
    STAmount result({currency, account});

    auto sleBixdState = view.read(keylet::line(account, issuer, currency));

    if (sleBixdState)
    {
        result = sleBixdState->getFieldAmount(
            account < issuer ? sfLowLimit : sfHighLimit);
        result.setIssuer(account);
    }

    assert(result.getIssuer() == account);
    assert(result.getCurrency() == currency);
    return result;
}

IOUAmount
creditLimit2(
    ReadView const& v,
    AccountID const& acc,
    AccountID const& iss,
    Currency const& cur)
{
    return toAmount<IOUAmount>(creditLimit(v, acc, iss, cur));
}

STAmount
creditBalance(
    ReadView const& view,
    AccountID const& account,
    AccountID const& issuer,
    Currency const& currency)
{
    STAmount result({currency, account});

    auto sleBixdState = view.read(keylet::line(account, issuer, currency));

    if (sleBixdState)
    {
        result = sleBixdState->getFieldAmount(sfBalance);
        if (account < issuer)
            result.negate();
        result.setIssuer(account);
    }

    assert(result.getIssuer() == account);
    assert(result.getCurrency() == currency);
    return result;
}

}  // namespace bixd
