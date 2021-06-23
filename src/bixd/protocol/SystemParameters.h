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

#ifndef BIXD_PROTOCOL_SYSTEMPARAMETERS_H_INCLUDED
#define BIXD_PROTOCOL_SYSTEMPARAMETERS_H_INCLUDED

#include <bixd/basics/XRPAmount.h>
#include <bixd/basics/chrono.h>
#include <cstdint>
#include <string>

namespace bixd {

// Various protocol and system specific constant globals.

/* The name of the system. */
static inline std::string const&
systemName()
{
    static std::string const name = "bixd";
    return name;
}

/** Configure the native currency. */

/** Number of drops in the genesis account. */
constexpr BIXRPAmount INITIAL_BIXRP{100'000'000'000 * DROPS_PER_BIXRP};

/** Returns true if the amount does not exceed the initial BIXRP in existence. */
inline bool
isLegalAmount(BIXRPAmount const& amount)
{
    return amount <= INITIAL_BIXRP;
}

/* The currency code for the native currency. */
static inline std::string const&
systemCurrencyCode()
{
    static std::string const code = "BIXRP";
    return code;
}

/** The BIXRP ledger network's earliest allowed sequence */
static std::uint32_t constexpr BIXRP_LEDGER_EARLIEST_SEQ{32570};

/** The minimum amount of support an amendment should have.

    @note This value is used by legacy code and will become obsolete
          once the fixAmendmentMajorityCalc amendment activates.
*/
constexpr std::ratio<204, 256> preFixAmendmentMajorityCalcThreshold;

constexpr std::ratio<80, 100> postFixAmendmentMajorityCalcThreshold;

/** The minimum amount of time an amendment must hold a majority */
constexpr std::chrono::seconds const defaultAmendmentMajorityTime = weeks{2};

}  // namespace bixd

/** Default peer port (IANA registered) */
inline std::uint16_t constexpr DEFAULT_PEER_PORT{2459};

#endif
