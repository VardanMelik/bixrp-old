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

#include <bixd/nodestore/impl/DatabaseNodeImp.h>
#include <bixd/nodestore/impl/ManagerImp.h>

#include <boost/algorithm/string/predicate.hpp>

namespace bixd {

namespace NodeStore {

ManagerImp&
ManagerImp::instance()
{
    static ManagerImp _;
    return _;
}

void
ManagerImp::missing_backend()
{
    Throw<std::runtime_error>(
        "Your bixd.cfg is missing a [node_db] entry, "
        "please see the bixd-example.cfg file!");
}

std::unique_ptr<Backend>
ManagerImp::make_Backend(
    Section const& parameters,
    std::size_t burstSize,
    Scheduler& scheduler,
    beast::Journal journal)
{
    std::string const type{get<std::string>(parameters, "type")};
    if (type.empty())
        missing_backend();

    auto factory{find(type)};
    if (!factory)
    {
#ifndef BIXD_REPORTING
        if (boost::iequals(type, "cassandra"))
            Throw<std::runtime_error>(
                "To use Cassandra as a nodestore, build bixd with "
                "-Dreporting=ON");
#endif
        missing_backend();
    }

    return factory->createInstance(
        NodeObject::keyBytes, parameters, burstSize, scheduler, journal);
}

std::unique_ptr<Database>
ManagerImp::make_Database(
    std::string const& name,
    std::size_t burstSize,
    Scheduler& scheduler,
    int readThreads,
    Stoppable& parent,
    Section const& config,
    beast::Journal journal)
{
    auto backend{make_Backend(config, burstSize, scheduler, journal)};
    backend->open();
    return std::make_unique<DatabaseNodeImp>(
        name,
        scheduler,
        readThreads,
        parent,
        std::move(backend),
        config,
        journal);
}

void
ManagerImp::insert(Factory& factory)
{
    std::lock_guard _(mutex_);
    list_.push_back(&factory);
}

void
ManagerImp::erase(Factory& factory)
{
    std::lock_guard _(mutex_);
    auto const iter =
        std::find_if(list_.begin(), list_.end(), [&factory](Factory* other) {
            return other == &factory;
        });
    assert(iter != list_.end());
    list_.erase(iter);
}

Factory*
ManagerImp::find(std::string const& name)
{
    std::lock_guard _(mutex_);
    auto const iter =
        std::find_if(list_.begin(), list_.end(), [&name](Factory* other) {
            return boost::iequals(name, other->getName());
        });
    if (iter == list_.end())
        return nullptr;
    return *iter;
}

//------------------------------------------------------------------------------

Manager&
Manager::instance()
{
    return ManagerImp::instance();
}

}  // namespace NodeStore
}  // namespace bixd
