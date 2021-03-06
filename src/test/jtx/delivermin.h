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

#ifndef BIXD_TEST_JTX_DELIVERMIN_H_INCLUDED
#define BIXD_TEST_JTX_DELIVERMIN_H_INCLUDED

#include <bixd/protocol/STAmount.h>
#include <test/jtx/Env.h>

namespace bixd {
namespace test {
namespace jtx {

/** Sets the DeliverMin on a JTx. */
class delivermin
{
private:
    STAmount amount_;

public:
    delivermin(STAmount const& amount) : amount_(amount)
    {
    }

    void
    operator()(Env&, JTx& jtx) const;
};

}  // namespace jtx
}  // namespace test
}  // namespace bixd

#endif
