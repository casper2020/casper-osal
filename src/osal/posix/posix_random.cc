/**
 * @file posix_random.cc - posix implementation of random number generator
 *
 * Based on code originally developed for NDrive S.A.
 *
 * Copyright (c) 2011-2018 Cloudware S.A. All rights reserved.
 *
 * This file is part of casper-osal.
 *
 * casper-osal is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * casper-osal is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with casper-osal.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <stdlib.h>

#include "osal/osalite.h"

void OsalRandomSeed (uint64_t a_seed)
{
    unsigned seed = static_cast<unsigned>(a_seed);
    srandom(seed);
}

int64_t OsalRandom ()
{
    return static_cast<int64_t>(random());
}

int64_t OsalRandomInRange (uint64_t a_range)
{
    return (OsalRandom() % static_cast<int64_t>(a_range)) + 1;
}
