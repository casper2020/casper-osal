#pragma once
/**
 * @file pow10.h declaration of Pow10 fast power of 10 with integer exponent
 *
 * Base class for objects that are parsed from JSON
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
#ifndef NRS_OSAL_UTIL_POW10_H
#define NRS_OSAL_UTIL_POW10_H

namespace osal
{
    namespace utils
    {
        double Pow10 (int a_power);
    }
}

#endif // NRS_OSAL_UTIL_POW10_H
