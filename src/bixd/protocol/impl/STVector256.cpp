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

#include <bixd/basics/Log.h>
#include <bixd/basics/StringUtilities.h>
#include <bixd/protocol/STVector256.h>
#include <bixd/protocol/jss.h>

namespace bixd {

STVector256::STVector256(SerialIter& sit, SField const& name) : STBase(name)
{
    Blob data = sit.getVL();
    auto const count = data.size() / (256 / 8);
    mValue.reserve(count);
    Blob::iterator begin = data.begin();
    unsigned int uStart = 0;
    for (unsigned int i = 0; i != count; i++)
    {
        unsigned int uEnd = uStart + (256 / 8);
        // This next line could be optimized to construct a default
        // uint256 in the vector and then copy into it
        mValue.push_back(uint256(Blob(begin + uStart, begin + uEnd)));
        uStart = uEnd;
    }
}

void
STVector256::add(Serializer& s) const
{
    assert(fName->isBinary());
    assert(fName->fieldType == STI_VECTOR256);
    s.addVL(mValue.begin(), mValue.end(), mValue.size() * (256 / 8));
}

bool
STVector256::isEquivalent(const STBase& t) const
{
    const STVector256* v = dynamic_cast<const STVector256*>(&t);
    return v && (mValue == v->mValue);
}

Json::Value STVector256::getJson(JsonOptions) const
{
    Json::Value ret(Json::arrayValue);

    for (auto const& vEntry : mValue)
        ret.append(to_string(vEntry));

    return ret;
}

}  // namespace bixd
