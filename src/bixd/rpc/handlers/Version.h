//------------------------------------------------------------------------------
/*
    This file is part of bixd
    Copyright (c) 2012, 2013 Bixd Labs Inc.

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

#ifndef BIXD_BIXD_RPC_HANDLERS_VERSION_H
#define BIXD_BIXD_RPC_HANDLERS_VERSION_H

#include <bixd/rpc/impl/RPCHelpers.h>

namespace bixd {
namespace RPC {

class VersionHandler
{
public:
    explicit VersionHandler(JsonContext&)
    {
    }

    Status
    check()
    {
        return Status::OK;
    }

    template <class Object>
    void
    writeResult(Object& obj)
    {
        setVersion(obj);
    }

    static char const*
    name()
    {
        return "version";
    }

    static Role
    role()
    {
        return Role::USER;
    }

    static Condition
    condition()
    {
        return NO_CONDITION;
    }
};

}  // namespace RPC
}  // namespace bixd

#endif
