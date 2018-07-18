/**
 * @file file.cc File handling for windows OS
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

#include "osal/osal_time.h"
#include "osal/osalite.h"
#include "osal/windows/windows_file.h"

#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#ifndef _WIN32
#include <unistd.h>
#endif
#include <time.h>
#include <fcntl.h>
#include <io.h>

osal::windows::File::File (const char* a_name) : osal::BaseFile(a_name)
{
    handle_ = INVALID_HANDLE_VALUE;
    mode_ = osal::File::EOpenModeNotSet;
}

osal::windows::File::~File ()
{
    Close();
}

osal::File::Status osal::windows::File::Open (const osal::File::OpenMode a_open_mode)
{
    // in use?
    if ( handle_ != INVALID_HANDLE_VALUE ) {
        return osal::File::EStatusOpenError;
    }
    // invalid params
    if ( name_ == NULL ) {
        return osal::File::EStatusOpenError;
    }
    // open
    WCHAR* w_name = Utf8ToWstr(name_);
    if (w_name == NULL) {
        return EStatusOutOfMemory;
    }
    if ( a_open_mode == osal::File::EOpenModeRead ) {
        handle_ = CreateFileW(w_name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    } else if ( a_open_mode == osal::File::EOpenModeWrite ) {
        handle_ = CreateFileW(w_name, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    } else {
        delete [] w_name;
        return osal::File::EStatusInvalidParams;
    }
    delete [] w_name;

    // keep track of the open mode
    if ( handle_ != INVALID_HANDLE_VALUE ) {
        mode_ = a_open_mode;
    } else {
        mode_ = EOpenModeNotSet;
    }
    // success?
    return handle_ != INVALID_HANDLE_VALUE ? osal::File::EStatusOk : osal::File::EStatusOpenError;
}

osal::File::Status osal::windows::File::Close ()
{
    // nothing to close?
    if ( handle_ == INVALID_HANDLE_VALUE ) {
        return osal::File::EStatusOk;
    }
    // close
    if ( CloseHandle(handle_) ) {
        handle_ = INVALID_HANDLE_VALUE;
    }
    //
    mode_ = osal::File::EOpenModeNotSet;
    // success?
    return handle_ == INVALID_HANDLE_VALUE ? osal::File::EStatusOk : osal::File::EStatusCloseError;
}

osal::File::Status osal::windows::File::Remove ()
{
    // open?
    if ( handle_ != INVALID_HANDLE_VALUE ) {
        return osal::File::EStatusInUse;
    }
    // still have file name?
    if ( name_ == NULL ) {
        return osal::File::EStatusNameError;
    }

    WCHAR* w_name = Utf8ToWstr(name_);
    if (w_name == NULL) {
        return EStatusOutOfMemory;
    }
    if ( DeleteFileW(w_name ) ) {
        // ok
        delete [] w_name;
        return osal::File::EStatusOk;
    } else {
        // can't remove
        delete [] w_name;
        return osal::File::EStatusRemoveError;
    }
}


osal::File::Status osal::windows::File::Read (void* o_buffer, const uint32_t a_size, uint32_t* o_size)
{
    DWORD lpNumberOfBytesRead;
    // not ready?
    if ( handle_ == INVALID_HANDLE_VALUE ) {
        return osal::File::EStatusFileNotOpen;
    }
    // read
    if ( ReadFile(handle_, o_buffer, a_size, &lpNumberOfBytesRead, NULL) == false ) {
        return osal::File::EStatusReadError;
    }
    if ( o_size != NULL) {
        *o_size = lpNumberOfBytesRead;
    }

    if ( lpNumberOfBytesRead != a_size ) {
        return osal::File::EStatusEndOfFile;
    }

    // success?
    return osal::File::EStatusOk;
}

osal::File::Status osal::windows::File::Write (const void* a_buffer, const uint32_t a_size, uint32_t* o_size)
{
    DWORD lpNumberOfBytesWritten;
    // not ready?
    if ( handle_ == INVALID_HANDLE_VALUE ) {
        return osal::File::EStatusFileNotOpen;
    }
    // write
    if ( WriteFile(handle_, a_buffer, a_size, &lpNumberOfBytesWritten, NULL) == false ) {
        return osal::File::EStatusWriteError;
    }

    if ( o_size != NULL ) {
        (*o_size) = lpNumberOfBytesWritten;
    }

    // success?
    return (uint32_t)lpNumberOfBytesWritten == a_size ? osal::File::EStatusOk : osal::File::EStatusWriteError;
}

osal::File::Status osal::windows::File::GetDescriptor (int* o_descriptor)
{
    if (o_descriptor == NULL) {
        return osal::File::EStatusInvalidParams;
    }

    // not ready?
    if ( handle_ == INVALID_HANDLE_VALUE ) {
        return osal::File::EStatusFileNotOpen;
    }

    //TODO: pcampos Review
     // Get the file descriptor from the handle
    if ( mode_ == osal::File::EOpenModeRead ) {
        *o_descriptor = _open_osfhandle((long)handle_, O_RDONLY);
    } else if ( mode_ == osal::File::EOpenModeWrite ) {
        *o_descriptor = _open_osfhandle((long)handle_, O_WRONLY);
    }

    // get decriptor
    //(*o_descriptor) = fileno(file_);
    // success?
    return osal::File::EStatusOk;
}

osal::File::Status osal::windows::File::ReadFromDescriptor (void* o_buffer, const uint32_t a_size, uint32_t* o_size)
{
    // TODO: pcampos whey not: assinc read?
    // return Read (o_buffer, a_size, o_size);

    // not ready?
    if ( handle_ == INVALID_HANDLE_VALUE ) {
        return osal::File::EStatusFileNotOpen;
    }
    //
    int descriptor;
    if ( GetDescriptor(&descriptor) != osal::File::EStatusOk ) {
        return osal::File::EStatusReadError;
    }
    // read
#ifdef _WIN32
    ssize_t read_bytes = 0;
#else
    ssize_t read_bytes = read(descriptor, o_buffer, a_size);
#endif
    //
    (*o_size) = read_bytes;
    //
    if ( read_bytes >= 0 ) {
        //
        if ( read_bytes == 0 || read_bytes < a_size ) {
            // end of file
            return osal::File::EStatusEndOfFile;
        } else {
            // continue
            return osal::File::EStatusOk;
        }
    } else if ( read_bytes == -1 ) {
        // try again?
        if ( errno == EAGAIN ) {
            // yes
            return osal::File::EStatusReadTryAgain;
        } else {
            // no
            return osal::File::EStatusReadError;
        }
    } else {
        // error
        return osal::File::EStatusReadError;
    }
}

osal::File::Status osal::windows::File::SetNonBlock ()
{
    // TODO: pcampos:
    // not ready?
    if ( handle_ == INVALID_HANDLE_VALUE ) {
        return osal::File::EStatusFileNotOpen;
    }
    // get decriptor
    //const int descriptor = fileno(file_);
    //
#ifndef _WIN32
    if ( fcntl(descriptor, F_SETFL, O_NONBLOCK) == 0 ) {
        return osal::File::EStatusOk;
    }
#endif
    //
    return osal::File::EStatusOk;
}

osal::File::Status osal::windows::File::IsOpen ()
{
    // not ready?
    if ( handle_ == INVALID_HANDLE_VALUE ) {
        return osal::File::EStatusFileNotOpen;
    }
    // ok
    return osal::File::EStatusOk;
}

osal::File::Status osal::windows::File::Exists ()
{
    if ( handle_ != INVALID_HANDLE_VALUE ) {
        return osal::File::EStatusOk;
    }
    return Exists(name_);
}

osal::File::Status osal::windows::File::Size (uint32_t* o_size)
{
    LARGE_INTEGER lpFileSize;

    // not ready?
    if ( name_ == NULL ) {
        return osal::File::EStatusNameError;
    }
    if (o_size == NULL) {
        return osal::File::EStatusInvalidParams;
    }

    // reset
    (*o_size) = 0;

    if ( GetFileSizeEx(handle_,&lpFileSize) ) {
        (*o_size) = lpFileSize.LowPart;
        return osal::File::EStatusOk;
    }
    //
    return osal::windows::File::Size(name_, o_size);
}

osal::File::Status osal::windows::File::Tell (uint32_t* a_postion)
{
    if (a_postion == NULL) {
        return osal::File::EStatusInvalidParams;
    }
    DWORD tmp = SetFilePointer(handle_,0, NULL,FILE_CURRENT);
    if ( tmp  == INVALID_SET_FILE_POINTER) {
        return osal::File::EStatusStatError;
    }
    *a_postion = tmp;
    return osal::File::EStatusOk;
}


osal::File::Status osal::windows::File::Seek (const uint32_t a_postion)
{
    if ( SetFilePointer(handle_,a_postion, NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
        return osal::File::EStatusStatError;
    }
    return osal::File::EStatusOk;
}

osal::File::Status osal::windows::File::GetLastAccessTime (int32_t* o_time)
{
    // not ready?
    if ( name_ == NULL ) {
        return osal::File::EStatusNameError;
    }
    //TODO: pcampos: ???
    struct stat stat_info;
    if ( stat(name_, &stat_info) == 0 ) {
        // set access time
        (*o_time) = (int32_t)stat_info.st_atime;
        // done
        return osal::File::EStatusOk;
    } else {
        return osal::File::EStatusStatError;
    }
}

osal::File::Status osal::windows::File::Delete (const char* a_name)
{
    // invalid name?
    if ( a_name == NULL ) {
        return osal::File::EStatusNameError;
    }
    WCHAR* w_name = Utf8ToWstr(a_name);
    if (w_name == NULL) {
        return EStatusOutOfMemory;
    }
    // try to remove this file
    if ( DeleteFileW(w_name) ) {
        // ok
        delete [] w_name;
        return osal::File::EStatusOk;
    }
    delete [] w_name;
    // can't remove
    return osal::File::EStatusRemoveError;
}

osal::File::Status osal::windows::File::Delete (const char* a_dir_name, const char* a_pattern, size_t* o_count)
{
    wchar_t pattern[MAX_PATH];
    wchar_t file_path[MAX_PATH];
    HANDLE hp;
    WIN32_FIND_DATA info;

    // open
    WCHAR* w_dir_name = Utf8ToWstr(a_dir_name);
    if (w_dir_name == NULL) {
        return EStatusOutOfMemory;
    }
    WCHAR* w_pattern = Utf8ToWstr(a_pattern);
    if (w_pattern == NULL) {
        delete [] w_dir_name;
        return EStatusOutOfMemory;
    }

    swprintf(pattern, MAX_PATH, L"%ls%ls", w_dir_name, w_pattern);
    hp = FindFirstFile(pattern, &info);
    do {
       swprintf(file_path, MAX_PATH, L"%ls%ls", w_dir_name, info.cFileName);
       DeleteFileW(file_path);
       if ( o_count != NULL ) {
         ++o_count;
       }
    } while(FindNextFile(hp, &info));
    FindClose(hp);
    delete [] w_dir_name;
    delete [] w_pattern;
    return osal::File::EStatusOk;
}

osal::File::Status osal::windows::File::Copy (const char* a_source, const char* a_destination)
{
    WCHAR* w_name_source = Utf8ToWstr(a_source);
    if (w_name_source == NULL) {
        return EStatusOutOfMemory;
    }
    WCHAR* w_name_destination = Utf8ToWstr(a_destination);
    if (w_name_destination == NULL) {
        delete [] w_name_source;
        return EStatusOutOfMemory;
    }

    if ( CopyFile(w_name_source, w_name_destination, false) ) {
        return osal::File::EStatusOk;
    }
    return osal::File::EStatusCopyError;
}

osal::File::Status osal::windows::File::Exists (const char* a_name)
{
    if ( a_name == NULL ) {
        return osal::File::EStatusNameError;
    }
    WCHAR* w_name = Utf8ToWstr(a_name);
    if (w_name == NULL) {
        return EStatusOutOfMemory;
    }
    if ( GetFileAttributes(w_name) == INVALID_FILE_ATTRIBUTES ) {
        delete [] w_name;
        return osal::File::EStatusDoesNotExist;
    }
    delete [] w_name;
    return osal::File::EStatusOk;
}

osal::File::Status osal::windows::File::Move (const char* a_source, const char* a_destination)
{
    WCHAR* w_name_source = Utf8ToWstr(a_source);
    if (w_name_source == NULL) {
        return EStatusOutOfMemory;
    }
    WCHAR* w_name_destination = Utf8ToWstr(a_destination);
    if (w_name_destination == NULL) {
        delete [] w_name_source;
        return EStatusOutOfMemory;
    }

    if ( MoveFile(w_name_source, w_name_destination) ) {
        return osal::File::EStatusOk;
    }
    return osal::File::EStatusCopyError;
}

osal::File::Status osal::windows::File::GetLastModificationTime (const char* a_name, int32_t* o_time)
{
    // not ready?
    if ( a_name == NULL ) {
        return osal::File::EStatusNameError; // TODO windows port
    }
    *(o_time) = osal::Time::k_invalid_time; // TODO windows port
    return osal::File::EStatusOk;
}

osal::File::Status osal::windows::File::Touch (const char* a_name)
{
    osal::windows::File f(a_name);

    osal::File::Status status = f.Open(osal::File::EOpenModeWrite);
    if ( status == osal::File::EStatusOk ) {
        status = f.Write("", 0, NULL);
        if ( status == osal::File::EStatusOk ) {
            status = f.Close();
        }
    }
    return status;
}

WCHAR* osal::windows::File::Utf8ToWstr (const char* a_str)
{
    LPWSTR  rv;
    int slength = strlen(a_str) + 1;
    rv = new WCHAR[MAX_PATH];
    MultiByteToWideChar(CP_UTF8, 0, a_str, slength, rv, MAX_PATH);
    return rv;
}

WCHAR* osal::windows::File::Utf8ToWstr (const char* a_str, WCHAR* o_str)
{
    int slength = strlen(a_str) + 1;
    MultiByteToWideChar(CP_UTF8, 0, a_str, slength, o_str, MAX_PATH);
    return o_str;
}

char* osal::windows::File::WstrToUtf8 (const WCHAR* a_w_str)
{
    char*  rv;
    int slength = wcslen(a_w_str) + 1;
    rv = new char[MAX_PATH];
    WideCharToMultiByte(CP_UTF8, 0, a_w_str, slength, rv, MAX_PATH, NULL, NULL);
    return rv;
}

char* osal::windows::File::WstrToUtf8 (const WCHAR* a_w_str, char* o_str)
{
    int slength = wcslen(a_w_str) + 1;
    WideCharToMultiByte(CP_UTF8, 0, a_w_str, slength, o_str, MAX_PATH, NULL, NULL);
    return o_str;
}

osal::File::Status osal::windows::File::Size (const char* a_name, uint32_t* o_size)
{
    // invalid name
    if ( a_name == NULL || o_size == NULL ) {
        return osal::File::EStatusNameError;
    }

    // reset
    (*o_size) = 0;

    // open
    WCHAR* w_name = Utf8ToWstr(a_name);
    if (w_name == NULL) {
        return EStatusOutOfMemory;
    }

    osal::File::Status rv = osal::File::EStatusDoesNotExist;

    //
    HANDLE tmp_handle = CreateFileW(w_name, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if ( tmp_handle != INVALID_HANDLE_VALUE ) {

        LARGE_INTEGER lpFileSize;
        if ( GetFileSizeEx(tmp_handle,&lpFileSize) ) {
            (*o_size) = lpFileSize.LowPart;
             rv = osal::File::EStatusOk;
        }

        if ( CloseHandle(tmp_handle) ) {
            tmp_handle = INVALID_HANDLE_VALUE;
        }
    }

    //
    delete [] w_name;

    // done
    return rv;
}

osal::windows::File::Status osal::windows::File::FindRecursive(const char* a_name, std::set<std::string> a_patterns, FindCallback* a_callback)
{
    WIN32_FIND_DATA ffd;
    HANDLE          find_handle;
    Status          rv;
    WCHAR*          w_name;
    DWORD           last_error;
    char            tmp[MAX_PATH];

    if ( Exists(a_name) == EStatusDoesNotExist ) {
        rv = EStatusDoesNotExist;
        goto return_rv;
    }

    find_handle = INVALID_HANDLE_VALUE;
    last_error  = ERROR_FILE_NOT_FOUND;

    for ( std::set<std::string>::iterator it = a_patterns.begin(); it != a_patterns.end(); ++it ) {

        snprintf(tmp, sizeof(tmp), "%s%s", a_name, (*it).c_str());

        w_name = Utf8ToWstr(tmp);
        if ( w_name == NULL ) {
            rv = EStatusOpenError;
            goto return_rv;
        }

        // Find the first file in the directory.
        find_handle = FindFirstFileW(w_name, &ffd);
        if (INVALID_HANDLE_VALUE == find_handle) {
            last_error = GetLastError();
            if ( ERROR_FILE_NOT_FOUND == last_error )  {
                continue; // no file found, try next pattern
            } else {
                rv = EStatusOpenError;
                goto return_rv;
            }
        }

        // List all the files in the directory with some info about them.
        do {
            if ( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                // skip
            } else {
                char* filename = WstrToUtf8(ffd.cFileName);
                if ( filename != NULL ) {
                    a_callback->OnNewFileEntry(a_name, filename);
                    delete [] filename;
                }
            }
        } while (FindNextFile(find_handle, &ffd) != 0);

        if ( find_handle != INVALID_HANDLE_VALUE ) {
            FindClose(find_handle);
            find_handle = INVALID_HANDLE_VALUE;
        }
        if ( w_name != NULL ) {
            delete [] w_name;
            w_name = NULL;
        }
    }

    if ( last_error == ERROR_NO_MORE_FILES || last_error == ERROR_FILE_NOT_FOUND ) {
        rv = EStatusOk;
    } else {
        rv = EStatusOpenError;
    }

return_rv:

    if ( find_handle != INVALID_HANDLE_VALUE ) {
        FindClose(find_handle);
    }
    if ( w_name != NULL ) {
        delete [] w_name;
    }
    return rv;
}

//endof $Id: $
