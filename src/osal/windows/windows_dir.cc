/**
 * @file windows_dir.cc - windows directory handling
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

#include "osal/windows/windows_dir.h"
#include "osal/windows/windows_file.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <direct.h>
#include <list>

#include "osal/debug_trace.h"


osal::windows::Dir::Status osal::windows::Dir::Exists (const char* a_name)
{
    if ( a_name == NULL ) {
        return EStatusOpenError;
    }
    WCHAR* w_name = osal::windows::File::Utf8ToWstr(a_name);
    if (w_name == NULL) {
        return EStatusOpenError;
    }

    DWORD attr = GetFileAttributes(w_name);
    if ( attr == INVALID_FILE_ATTRIBUTES || !(attr & FILE_ATTRIBUTE_DIRECTORY) ) {
        delete [] w_name;
        return EStatusDoesNotExist;
    }
    delete [] w_name;
    return EStatusOk;
}


osal::windows::Dir::Status osal::windows::Dir::CreatePath (const char* a_path)
{
    char*    path      = (char*)a_path;
    char*    tmp       = NULL;
    char*    full_path  = NULL;
    bool     bContinue = true;
    int32_t  k         = 0;

    // log
    DEBUGTRACE("create_path", "%-96s", a_path);

    // nothing to create?
    if ( a_path == NULL ) {
        return osal::windows::Dir::EStatusOk;
    }
    // already exists?
    if ( osal::windows::Dir::Exists(a_path) == osal::windows::Dir::EStatusOk ) {
        // yes
        return osal::windows::Dir::EStatusOk;
    }
    // try to create
    if ( osal::windows::Dir::CreateDir(a_path) == osal::windows::Dir::EStatusOk ) {
        // succeeded
        return osal::windows::Dir::EStatusOk;
    }
    //
    k         = strlen(a_path) + 1;
    full_path = (char*)malloc(sizeof(char)*( 2 * k ));
    //
    if ( full_path != NULL ) {
        tmp = (char*)malloc(sizeof(char)*k);
        if ( tmp != NULL ) {
            // create the full path
            *full_path = 0;
            //strcat(full_path,"/");
            while ( bContinue == true && path != NULL && strlen(path)>0 ) {
                // next...
                *tmp = 0;
                while ( path != NULL && strlen(path)>0 && path[0] != '/' && path[0] != '\\'  ) {
                    strncat(tmp, path, 1);
                    path++;
                }
                if ( path != NULL && strlen(path) > 0 && (path[0] == '/' || path[0] == '\\') ) {
                    path++;
                }
                //create the path
                if ( tmp != NULL && strlen(tmp) > 0 && tmp[0] != '/' && tmp[0] != '\\' ) {
                    strcat(full_path,tmp);
                    if ( osal::windows::Dir::Exists(full_path) == osal::windows::Dir::EStatusDoesNotExist) {
                        bContinue = osal::windows::Dir::CreateDir(full_path) == osal::windows::Dir::EStatusOk;
                    }
                    strcat(full_path,"/");
                }
                //more dirs to create ?
                if ( path == NULL || strlen(path) == 0 ) {
                    bContinue = false;
                    break;
                }
            }
            free(tmp);
        }
        free(full_path);
    }
    // done
    return osal::windows::Dir::Exists(a_path);
}

osal::windows::Dir::Status osal::windows::Dir::CreateDir (const char* a_name)
{
    WCHAR* w_name = osal::windows::File::Utf8ToWstr(a_name);
    if (w_name == NULL) {
        return EStatusCreateFailed;
    }
    if ( CreateDirectoryW(w_name, NULL) ) {
        delete [] w_name;
        return osal::windows::Dir::EStatusOk;
    }
    delete [] w_name;

    const DWORD error_number =  GetLastError();
    if ( error_number == ERROR_ALREADY_EXISTS ) {
        DEBUGTRACE("create_dir", "'%-96s' : %s", a_name, "already exists");
        return osal::windows::Dir::EStatusOk;
    } else {
        DEBUGTRACE("create_dir", "'%-96s' : %s ( %d ) ", a_name, "failed", error_number);
    }
    return osal::windows::Dir::EStatusCreateFailed;
}

osal::windows::Dir::Status osal::windows::Dir::Delete (const char* a_path)
{
    BOOL rv = false;
    if ( a_path == NULL ) {
        return EStatusOpenError;
    }
    WCHAR* w_path = osal::windows::File::Utf8ToWstr(a_path);
    if (w_path == NULL) {
        return EStatusOpenError;
    }
    rv = RemoveDirectoryW(w_path);
    delete [] w_path;

    return rv ? osal::windows::Dir::EStatusOk:osal::windows::Dir::EStatusOpenError;
}


osal::windows::Dir::Status osal::windows::Dir::FreeOrTotalSpace (const char* a_name, int64_t* o_space, bool a_free_space)
{
    ULARGE_INTEGER freeBytesAvailableToCaller;
    ULARGE_INTEGER totalNumberOfBytes;
    ULARGE_INTEGER totalNumberOfFreeBytes;

    WCHAR* w_path = osal::windows::File::Utf8ToWstr(a_name);
    if ( w_path == NULL ) {
        return EStatusOpenError;
    }
    if ( GetDiskFreeSpaceExW(w_path, &freeBytesAvailableToCaller, &totalNumberOfBytes, &totalNumberOfFreeBytes ) == FALSE ) {
        delete [] w_path;
        return EStatusOpenError;
    }
    if ( a_free_space == true ) {
        *(o_space) = freeBytesAvailableToCaller.QuadPart;
    } else {
        *(o_space) = totalNumberOfBytes.QuadPart;
    }
    delete [] w_path;
    return EStatusOk;
}

