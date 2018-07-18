#pragma once
/**
 * @file file.h
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
#ifndef NRS_OSAL_POSIX_POSIX_FILE
#define NRS_OSAL_POSIX_POSIX_FILE

#include "osal/base_file.h"

#include <stdio.h>

#include <set>
#include <string>

namespace osal {

    namespace posix {

        class File : public osal::BaseFile {

        protected: // data

            FILE* file_;

        protected: // static data

            static Status g_last_status_;  // @TODO check thread safety

        public: // constructor(s) / destructor

            File          (const char* a_name);
            virtual ~File ();

        public: // method(s) / function(s)

            // open / close
            Status Open   (const OpenMode a_open_mode);
            Status Close  ();
            Status Remove ();

            // read / write
            Status Read  (void* o_buffer, const uint32_t a_size, uint32_t* o_size);
            Status Write (const void* a_buffer, const uint32_t a_size, uint32_t* o_size);
            Status Write (const char* a_buffer, const uint32_t a_size, uint32_t* o_size);

            // descriptor
            Status GetDescriptor      (int* o_descriptor);
            Status ReadFromDescriptor (void* o_buffer, const uint32_t a_size, uint32_t* o_size);
            Status SetNonBlock        ();

            // status
            Status   IsOpen            ();
            Status   Exists            ();
            Status   Size              (uint32_t* o_size);
            Status   GetLastAccessTime (int32_t* o_time);
            OpenMode GetOpenMode       ();

            Status   Tell              (uint32_t* a_postion);
            Status   Seek              (const uint32_t a_postion);

            Status   Rewind            (const uint32_t& a_bytes);

        public: // static method(s) / function(s) declaration

            static Status Delete                  (const char* a_name);
            static Status Delete                  (const char* a_dir_name, const char* a_pattern, size_t* o_count);
            static bool   Exists                  (const char* a_name);
            static Status Copy                    (const char* a_source, const char* a_destination);
            static Status Move                    (const char* a_source, const char* a_destination);
            static Status GetLastModificationTime (const char* a_name, int32_t* o_time);
            static Status Size                    (const char* a_name, uint32_t* o_size);
            static Status Touch                   (const char* a_name);
            static Status FindRecursive           (const char* a_name, std::set<std::string> a_patterns, FindCallback* a_callback);
            static Status UniqueFileName          (const std::string& a_path, const std::string& a_prefix, const std::string& a_extension, std::string& o_name);
            static Status Basename                (const char* a_uri, std::string& o_name);

        };

        inline File::OpenMode File::GetOpenMode ()
        {
            return mode_;
        }

    };

}

#endif // NRS_OSAL_POSIX_POSIX_FILE_H
