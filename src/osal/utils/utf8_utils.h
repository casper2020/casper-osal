#pragma once
/**
 * @file utf8_utils.h - Helper to handle UTF8 on the metal
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
#ifndef NRS_OSAL_UTILS_UTF8_UTILS_H
#define NRS_OSAL_UTILS_UTF8_UTILS_H

#include <string>
#include <vector>
#include <map>

#define UTF8_COLLATION_MAX_INDEX 1792

namespace osal {

    namespace utils {

        /**
         * @brief A helper class that contains some UTF8 tables.
         */
        class UTF8Utils
        {

        public: // static const data

            static const uint16_t k_utf8_collation_table_       [UTF8_COLLATION_MAX_INDEX];
            static const uint16_t k_utf8_collation_table_lower_ [UTF8_COLLATION_MAX_INDEX];
            static const uint16_t k_utf8_collation_table_upper_ [UTF8_COLLATION_MAX_INDEX];

        };
    }
}
#endif // NRS_OSAL_UTILS_UTF8_UTILS_H
