//------------------------------------------------------------------------------
/*
    This file is part of bixd
    Copyright (c) 2016 bixd Labs Inc.

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

#ifndef BIXD_APP_MISC_DETAIL_WORKSSL_H_INCLUDED
#define BIXD_APP_MISC_DETAIL_WORKSSL_H_INCLUDED

#include <bixd/app/misc/detail/WorkBase.h>
#include <bixd/basics/contract.h>
#include <bixd/core/Config.h>
#include <bixd/net/HTTPClientSSLContext.h>
#include <boost/asio/ssl.hpp>
#include <boost/format.hpp>

#include <functional>

namespace bixd {

namespace detail {

// Work over SSL
class WorkSSL : public WorkBase<WorkSSL>,
                public std::enable_shared_from_this<WorkSSL>
{
    friend class WorkBase<WorkSSL>;

private:
    using stream_type = boost::asio::ssl::stream<socket_type&>;

    HTTPClientSSLContext context_;
    stream_type stream_;

public:
    WorkSSL(
        std::string const& host,
        std::string const& path,
        std::string const& port,
        boost::asio::io_service& ios,
        beast::Journal j,
        Config const& config,
        endpoint_type const& lastEndpoint,
        bool lastStatus,
        callback_type cb);
    ~WorkSSL() = default;

private:
    stream_type&
    stream()
    {
        return stream_;
    }

    void
    onConnect(error_code const& ec);

    void
    onHandshake(error_code const& ec);
};

}  // namespace detail

}  // namespace bixd

#endif
