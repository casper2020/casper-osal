#pragma once
/**
 *  @file random.h
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
 * along with osal.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef NRS_OSAL_OSAL_RANDOM_H_
#define NRS_OSAL_OSAL_RANDOM_H_

#include "osal/osal_types.h"

extern "C" {

    void OsalRandomSeed (uint64_t a_seed);

    /**
     * @brief Generates a random number using
     * the seed provided by ::OsalRandomSeed
     * @return the random generated number
     */
    int64_t OsalRandom ();

    /**
     * @brief Generates a random number using
     * the seed provided by ::OsalRandomSeed
     * between 1 and <code>a_range</code>
     * @return the random generated number
     */
    int64_t OsalRandomInRange (uint64_t a_range);
}

#endif // NRS_OSAL_OSAL_RANDOM_H_
