#pragma once
/**
 * @file windows_dir.h - posix
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

#ifndef NRS_OSAL_WINDOWS_WINDOWS_DIR_H
#define NRS_OSAL_WINDOWS_WINDOWS_DIR_H

#include "osal/osalite.h"

#include <stdint.h>
#include <vector>
#include <string>
#include <limits.h>
#ifdef _WIN32
#define PATH_MAX MAX_PATH
#endif

namespace osal {
    namespace windows {

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
            static Status Delete     (const char* a_path);
            static Status FreeSpace  (const char* a_name, int64_t* o_space);
            static Status TotalSpace (const char* a_name, int64_t* o_space);

        protected:

            static Status FreeOrTotalSpace (const char* a_name, int64_t* o_space, bool a_free_space);
        };

    }

    inline osal::windows::Dir::Status osal::windows::Dir::FreeSpace (const char* a_name, int64_t* o_space)
    {
        return FreeOrTotalSpace(a_name, o_space, true);
    }

    inline osal::windows::Dir::Status osal::windows::Dir::TotalSpace (const char* a_name, int64_t* o_space)
    {
        return FreeOrTotalSpace(a_name, o_space, false);
    }

}

#endif // NRS_OSAL_WINDOWS_WINDOWS_DIR_H
