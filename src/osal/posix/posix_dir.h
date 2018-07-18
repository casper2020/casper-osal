#pragma once
/**
 * @file posix_dir.h - posix
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
 */
#ifndef NRS_SKUNK_OSAL_POSIX_POSIX_DIR_H
#define NRS_SKUNK_OSAL_POSIX_POSIX_DIR_H

#include <stdint.h>
#include <vector>
#include <string>
#include <functional>
#include <limits.h>
#ifdef _WIN32
    #define PATH_MAX MAX_PATH
#endif

namespace osal {
    namespace posix {

        class Dir {

        public: // enums

            // file read
            typedef enum {
                EStatusOk,
                EStatusDoesNotExist,
                EStatusOpenError,
                EStatusCreateFailed,
            } Status;

        public: // static data

            static const size_t kMaxPath = PATH_MAX;

        public: // static method(s) / function(s) declaration

            static Status CreatePath (const char* a_path);
            static Status Exists     (const char* a_path);
            static Status CreateDir  (const char* a_path);
            static Status Find       (const char* a_name, const char* a_pattern,
                                      std::vector<std::string>* o_results, volatile bool& a_abort);
            static Status FindFile   (const char* a_dir, const char* a_pattern, const std::function<bool(const std::string& a_uri)> a_callback);
            static Status Delete     (const char* a_name);

            static Status FreeSpace  (const char* a_name, int64_t* o_space);
            static Status TotalSpace (const char* a_name, int64_t* o_space);

        };
    }
}

#endif // NRS_SKUNK_OSAL_POSIX_POSIX_DIR_H
