#pragma once
/**
 * @file base_file.h Common base class for file implementations
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
#ifndef NRS_OSAL_BASE_FILE_H
#define NRS_OSAL_BASE_FILE_H

#include <stdlib.h> // free
#include <string.h> // strdup
#include <set>

namespace osal {

    class BaseFile {

    public: // typedefs

        class FindCallback {

            public: // constructor(s) / destructor

                virtual ~FindCallback ()
                {

                }

            public: // pure virtual method(s) declaration

            virtual bool OnNewFileEntry (const char* a_dir_name, const char* a_file_name) = 0;

        };


        // open mode
        typedef enum {
            EOpenModeRead,
            EOpenModeWrite,
            EOpenModeNotSet,
        } OpenMode;

        // file read
        typedef enum {
            EStatusOk,
            EStatusInvalidParams,
            EStatusFileNotOpen,
            EStatusEndOfFile,
            EStatusOpenError,
            EStatusCloseError,
            EStatusReadError,
            EStatusWriteError,
            EStatusInUse,
            EStatusStatError,
            EStatusNameError,
            EStatusRemoveError,
            EStatusDoesNotExist,
            EStatusNonBlockSetError,
            EStatusReadTryAgain,
            EStatusCreateFailed,
            EStatusCopyError,
            EStatusOutOfMemory,
            EStatusSeekError
        } Status;

    protected: // data

        char*    name_;    //!< file name ( path included )
        OpenMode mode_;    //!< file open mode

    public:

                BaseFile  (const char* a_name);
        virtual ~BaseFile ();

    public: // methods

        const char*  name  () const;
        OpenMode     mode  () const;

    };

    inline BaseFile::BaseFile (const char* a_name)
    {
        if ( a_name != NULL ) {
            name_ = strdup(a_name);
        } else {
            name_ = NULL;
        }
        mode_ = EOpenModeNotSet;
    }

    inline BaseFile::OpenMode BaseFile::mode () const
    {
        return mode_;
    }

    inline const char* BaseFile::name () const
    {
        return name_;
    }

} // namespace osal

#endif // NRS_OSAL_BASE_FILE_H
