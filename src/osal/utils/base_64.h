#pragma once
/**
 * @file Base64.h - Base 64 converter - RFC 3548 implementation ( NOT MIME ).
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
#ifndef NRS_OSAL_UTILS_BASE_64_H
#define NRS_OSAL_UTILS_BASE_64_H

#include <stdint.h>
#include <sys/types.h>

#include <string> // std::string

namespace osal {

    namespace utils {

        class Base64 {

        public: // enumerations

            typedef enum {
                EStatusOk,
                EStatusInvalidParams,
                EStatusOutOfMemory,
                EStatusDecodeError,
            } Status;

        protected: // const data

            static const char kTable64 [];

        public: // method(s) / function(s) declaration

            static Status Encode        (const char* a_buffer, size_t a_buffer_size, char** o_buffer, size_t& o_buffer_size);
            static Status Encode        (const std::string& a_buffer, std::string& o_buffer);
            static Status Decode        (const char* a_buffer, uint8_t** o_buffer, size_t& o_buffer_size);
            static Status Decode        (const std::string& a_buffer, std::string& o_buffer);
            static void   Release       (char** o_buffer);
            static void   Release       (uint8_t** o_buffer);

        private: // method(s) / function(s) declaration

            static Status DecodeQuantum (const char* a_source, uint8_t* o_destination);
        };
    }
}

#endif // NRS_OSAL_UTILS_BASE_64_H
