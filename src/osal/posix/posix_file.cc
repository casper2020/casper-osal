/**
 * @file file.cc - Implementation of file abstraction for posix compliant systems
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

#include "osal/osalite.h"
#include "osal/posix/posix_file.h"

#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <fnmatch.h>
#include <dirent.h>

#if defined(ANDROID)
#include <sys/statfs.h>
#else
#include <sys/statvfs.h>
#endif

#include <time.h>

#include <libgen.h> // basename

#include "osal/osal_dir.h"
#include "osal/debug_trace.h"


osal::File::Status osal::posix::File::g_last_status_ = osal::File::EStatusOk;

osal::posix::File::File (const char* a_name) : osal::BaseFile(a_name)
{
    file_ = NULL;
    mode_ = osal::File::EOpenModeNotSet;
}

osal::posix::File::~File ()
{
    Close();
}

osal::File::Status osal::posix::File::Open (const osal::File::OpenMode a_open_mode)
{
    // in use?
    if ( file_ != NULL ) {
        return osal::File::EStatusOpenError;
    }
    // invalid params
    if ( name_ == NULL ) {
        return osal::File::EStatusOpenError;
    }
    // open
    if ( a_open_mode == osal::File::EOpenModeRead ) {
        file_ = fopen(name_, "r");
    } else if ( a_open_mode == osal::File::EOpenModeWrite ) {
        file_ = fopen(name_, "w");
    } else {
        return osal::File::EStatusInvalidParams;
    }
    // keep track of the open mode
    if ( file_ != NULL ) {
        mode_ = a_open_mode;
    } else {
        mode_ = EOpenModeNotSet;
    }
    // success?
    return file_ != NULL ? osal::File::EStatusOk : osal::File::EStatusOpenError;
}

osal::File::Status osal::posix::File::Close ()
{
    // nothing to close?
    if ( file_ == NULL ) {
        return osal::File::EStatusOk;
    }
    // close
    if ( fclose(file_) == 0 ) {
        file_ = NULL;
    }
    //
    mode_ = osal::File::EOpenModeNotSet;
    // success?
    return file_ == NULL ? osal::File::EStatusOk : osal::File::EStatusCloseError;
}

osal::File::Status osal::posix::File::Remove ()
{
     // open?
    if ( file_ != NULL ) {
        return osal::File::EStatusInUse;
    }
    // still have file name?
    if ( name_ == NULL ) {
        return osal::File::EStatusNameError;
    }
    // try to remove this file
    if ( unlink(name_) == 0 ) {
        // ok
        return osal::File::EStatusOk;
    } else {
        // can't remove
        return osal::File::EStatusRemoveError;
    }
}


osal::File::Status osal::posix::File::Read (void* o_buffer, const uint32_t a_size, uint32_t* o_size)
{
    // not ready?
    if ( file_ == NULL ) {
        return osal::File::EStatusFileNotOpen;
    }
    // read
    (*o_size) = (uint32_t) fread(o_buffer, 1, a_size, file_);
    if ( (*o_size) != a_size ) {
        if ( feof(file_) != 0 ) {
            return osal::File::EStatusEndOfFile;
        } else {
            return osal::File::EStatusReadError;
        }
    }
    // success?
    return osal::File::EStatusOk;
}

osal::File::Status osal::posix::File::Write (const void* a_buffer, const uint32_t a_size, uint32_t* o_size)
{
    // not ready?
    if ( file_ == NULL ) {
        return osal::File::EStatusFileNotOpen;
    }
    // write
    const size_t written = fwrite(a_buffer, 1, a_size, file_);
    if ( o_size != NULL ) {
        (*o_size) = (uint32_t) written;
    }

    // success?
    return (uint32_t)written == a_size ? osal::File::EStatusOk : osal::File::EStatusWriteError;
}

osal::File::Status osal::posix::File::Write (const char* a_buffer, const uint32_t a_size, uint32_t* o_size)
{
    // not ready?
    if ( file_ == NULL ) {
        return osal::File::EStatusFileNotOpen;
    }
    // write
    const int written = fprintf(file_, "%*.*s", a_size, a_size, a_buffer);
    if ( written < 0 || ( written > static_cast<int>(a_size) ) ) {
        return osal::File::EStatusWriteError;
    }
    if ( o_size != NULL ) {
        (*o_size) = static_cast<uint32_t>(written);
    }
    // success?
    return static_cast<uint32_t>(written) == a_size ? osal::File::EStatusOk : osal::File::EStatusWriteError;
}

osal::File::Status osal::posix::File::GetDescriptor (int* o_descriptor)
{
     // not ready?
    if ( file_ == NULL ) {
        return osal::File::EStatusFileNotOpen;
    }
    // get decriptor
    (*o_descriptor) = fileno(file_);
    // failure?
    if ( (*o_descriptor) == -1 ) {
        return osal::File::EStatusOpenError;
    } else {
        return osal::File::EStatusOk;
    }
}

osal::File::Status osal::posix::File::ReadFromDescriptor (void* o_buffer, const uint32_t a_size, uint32_t* o_size)
{
    // not ready?
    if ( file_ == NULL ) {
        return osal::File::EStatusFileNotOpen;
    }
    //
    int descriptor;
    if ( GetDescriptor(&descriptor) != osal::File::EStatusOk ) {
        return osal::File::EStatusReadError;
    }
    // read
    ssize_t read_bytes = read(descriptor, o_buffer, a_size);
    //
    (*o_size) = (uint32_t) read_bytes;
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

osal::File::Status osal::posix::File::SetNonBlock ()
{
    // not ready?
    if ( file_ == NULL ) {
        return osal::File::EStatusFileNotOpen;
    }
    //
#ifdef __APPLE__ // TARGET_OS_IPHONE==1
    return osal::File::EStatusOk;
#else
    // get decriptor
    const int descriptor = fileno(file_);
    if ( fcntl(descriptor, F_SETFL, O_NONBLOCK) == 0 ) {
        return osal::File::EStatusOk;
    }
#endif
    //
    return osal::File::EStatusNonBlockSetError;
}

osal::File::Status osal::posix::File::IsOpen ()
{
     // not ready?
    if ( file_ == NULL ) {
        return osal::File::EStatusFileNotOpen;
    }
    // ok
    return osal::File::EStatusOk;
}

osal::File::Status osal::posix::File::Exists ()
{
    // not ready?
    if ( name_ == NULL ) {
        return osal::File::EStatusNameError;
    }
    // exits?
    struct stat stat_info;
    if ( stat(name_, &stat_info) == 0 ) {
        if ( S_ISREG(stat_info.st_mode) != 0 ) {
            return osal::File::EStatusOk;
        } else {
            return osal::File::EStatusDoesNotExist;
        }
    } else {
        return osal::File::EStatusStatError;
    }
}

osal::File::Status osal::posix::File::Size (uint32_t* o_size)
{
    // not ready?
    if ( name_ == NULL ) {
        return osal::File::EStatusNameError;
    }
    // reset
    (*o_size) = 0;
    // exits?
    struct stat stat_info;
    if ( stat(name_, &stat_info) == 0 ) {
        // exists?
#ifndef _WIN32
        if ( S_ISREG(stat_info.st_mode) != 0 ) {
            // set size
            (*o_size) = (uint32_t) stat_info.st_size;
            // ok
            return osal::File::EStatusOk;
        } else {
            // doesn't exists
            return osal::File::EStatusDoesNotExist;
        }
#endif
    } else {
        return osal::File::EStatusStatError;
    }
}

osal::File::Status osal::posix::File::Tell (uint32_t* a_postion)
{
    if ( file_ == NULL ) {
        return osal::File::EStatusFileNotOpen;
    }

	if (a_postion == NULL) {
		return osal::File::EStatusInvalidParams;
	}

	(*a_postion) = (uint32_t) ftell(file_);

	return osal::File::EStatusOk;
}


osal::File::Status osal::posix::File::Seek (const uint32_t a_postion)
{
	if ( fseek(file_, a_postion, SEEK_SET) != 0) {
		return osal::File::EStatusSeekError;
	}
	return osal::File::EStatusOk;
}

osal::File::Status osal::posix::File::Rewind (const uint32_t& a_bytes)
{
    if ( file_ == NULL ) {
        return osal::File::EStatusFileNotOpen;
    }

    const long current_position = ftell(file_);
    if ( current_position < static_cast<long>(a_bytes) ) {
        return osal::File::EStatusSeekError;
    }

    if ( 0 != fseek(file_, current_position - static_cast<long>(a_bytes) , SEEK_SET) ) {
        return osal::File::EStatusStatError;
    }

    return osal::File::EStatusOk;
}

osal::File::Status osal::posix::File::GetLastAccessTime (int32_t* o_time)
{
    // not ready?
    if ( name_ == NULL ) {
        return osal::File::EStatusNameError;
    }
    //
    struct stat stat_info;
    if ( stat(name_, &stat_info) == 0 ) {
        // get access time
        (*o_time) = (int32_t)stat_info.st_atime;
        // done
        return osal::File::EStatusOk;
    } else {
        return osal::File::EStatusStatError;
    }
}

osal::File::Status osal::posix::File::Delete (const char* a_name)
{
    // invalid name?
    if ( a_name == NULL ) {
        return osal::File::EStatusNameError;
    }
    // try to remove this file
    if ( unlink(a_name) == -1 ) {
		if ( errno == ENOENT ) {
			return osal::File::EStatusOk;
		} else {
			return osal::File::EStatusRemoveError;
		}
	}
	return osal::File::EStatusOk;
}

osal::File::Status osal::posix::File::Delete (const char* a_dir_name, const char* a_pattern, size_t* o_count)
{
    DIR*   dir_handle                  = NULL;
    char   path [osal::Dir::kMaxPath] = {0, 0};
    // invalid name?
    if ( a_dir_name == NULL || a_pattern == NULL ) {
        return osal::File::EStatusNameError;
    }
    //
	dir_handle = opendir(a_dir_name);
	if ( dir_handle == NULL ) {
		return osal::File::EStatusOpenError;
	}
	//
	struct dirent* dir_entry;
	while ( (dir_entry = readdir(dir_handle)) ) {
		if ( dir_entry->d_type & DT_REG ) {
			if ( fnmatch(a_pattern, dir_entry->d_name, FNM_CASEFOLD) == 0 ) {
                snprintf(path, sizeof(path), "%s%s", a_dir_name, dir_entry->d_name);
                if ( osal::File::Delete(path) == osal::File::EStatusOk ) {
                    if ( o_count != NULL ) {
                        (*o_count)++;
                    }
                }
			}
		}
	}
    //
	closedir(dir_handle);
    dir_handle = NULL;
    //
    return osal::File::EStatusOk;
}

osal::File::Status osal::posix::File::Copy (const char* a_source, const char* a_destination)
{
    uint32_t              src_file_size = 0;
    uint32_t              copied_bytes  = 0;
    uint32_t              read_size     = 0;
    uint32_t              write_size    = 0;
    uint8_t*              bytes_buffer  = NULL;

    static const uint32_t MAX_BUFFER_SIZE = 4096;

    osal::posix::File    src(a_source);
    osal::posix::File    dst(a_destination);

    osal::File::Status   copy_status = osal::File::EStatusCopyError;
    osal::File::Status   read_status = osal::File::EStatusOk;

    // open read file
    if ( src.Open(osal::File::EOpenModeRead) != osal::File::EStatusOk ) {
        copy_status = osal::File::EStatusOpenError;
        goto Copy_cleanup;
    }

    // get source size
    if ( src.Size(&src_file_size) != osal::File::EStatusOk ) {
        copy_status = osal::File::EStatusReadError;
        goto Copy_cleanup;
    }

    // open write file
    if ( dst.Open(osal::File::EOpenModeWrite) != osal::File::EStatusOk ) {
        copy_status = osal::File::EStatusWriteError;
        goto Copy_cleanup;
    }

    // create buffer
    bytes_buffer = (uint8_t*)malloc(sizeof(uint8_t)*MAX_BUFFER_SIZE);
    if ( bytes_buffer == NULL ) {
        copy_status = osal::File::EStatusOutOfMemory;
        goto Copy_cleanup;
    }

    // copy loop
    for ( copied_bytes = 0; copied_bytes < src_file_size ; ) {
        // read from source
        read_status = src.Read(bytes_buffer, MAX_BUFFER_SIZE, &read_size);
        if ( read_status != osal::File::EStatusOk && read_status != osal::File::EStatusEndOfFile ) {
            copy_status = osal::File::EStatusReadError;
            goto Copy_cleanup;
        }
        // write to destination
        if ( dst.Write(bytes_buffer, read_size, &write_size) != osal::File::EStatusOk ) {
            copy_status = osal::File::EStatusWriteError;
            goto Copy_cleanup;
        }
        // not written the same number of bytes?
        if ( read_size != write_size ) {
            copy_status = osal::File::EStatusWriteError;
            goto Copy_cleanup;
        }
        // next...
        copied_bytes += read_size;
    }

    // set copy status
    copy_status = copied_bytes == src_file_size ? osal::File::EStatusOk : osal::File::EStatusCopyError;

Copy_cleanup:

    // close source file
    src.Close();

    // close destination file
    dst.Close();

    // dispose buffer
    if (bytes_buffer != NULL) {
        free(bytes_buffer);
        bytes_buffer = NULL;
    }

    // done
    return copy_status;
}

/**
 * @brief Static member to if a file exists
 *
 * @param a_name name of the file
 * @return true the file exists, false the file does not exist
 */
bool osal::posix::File::Exists (const char* a_name)
{
    // not ready?
    if ( a_name == NULL ) {
        g_last_status_ = osal::File::EStatusNameError;
        return false;
    }
    // exits?
    struct stat stat_info;
    if ( stat(a_name, &stat_info) == 0 ) {
        if ( S_ISREG(stat_info.st_mode) != 0 ) {
            g_last_status_ = osal::File::EStatusOk;
            return true;
        } else {
            g_last_status_ = osal::File::EStatusDoesNotExist;
            return false;
        }
    } else {
        g_last_status_ = osal::File::EStatusStatError;
        return false;
    }
}

osal::File::Status osal::posix::File::Move (const char* a_source, const char* a_destination)
{
    if ( rename(a_source, a_destination) == 0 ) {
        return osal::File::EStatusOk;
    } else {
        return osal::File::EStatusStatError;
    }
}

osal::File::Status osal::posix::File::GetLastModificationTime (const char* a_name, int32_t* o_time)
{
    // not ready?
    if ( a_name == NULL ) {
        return osal::File::EStatusNameError;
    }
    //
    struct stat stat_info;
    if ( stat(a_name, &stat_info) == 0 ) {
        // get access time
        time_t local = stat_info.st_mtime;
        // convert to UTC
        struct tm      tm;
        if ( gmtime_r(&local, &tm) == &tm ) {
            //
            (*o_time) = (int32_t)mktime(&tm);
            // done
            return osal::File::EStatusOk;
        } else {
            return osal::File::EStatusStatError;
        }
    } else {
        return osal::File::EStatusStatError;
    }
}

osal::File::Status osal::posix::File::Size (const char* a_name, uint32_t* o_size)
{
    // not ready?
    if ( a_name == NULL ) {
        return osal::File::EStatusNameError;
    }
    // reset
    (*o_size) = 0;
    // exits?
    struct stat stat_info;
    if ( stat(a_name, &stat_info) == 0 ) {
        // exists?
        if ( S_ISREG(stat_info.st_mode) != 0 ) {
            // set size
            (*o_size) = (uint32_t) stat_info.st_size;
            // ok
            return osal::File::EStatusOk;
        } else {
            // doesn't exists
            return osal::File::EStatusDoesNotExist;
        }
    } else {
        return osal::File::EStatusStatError;
    }
}

osal::File::Status osal::posix::File::Touch (const char* a_name)
{
    osal::posix::File f (a_name);
    //
    osal::File::Status status = f.Open(osal::File::EOpenModeWrite);
    if ( status == osal::File::EStatusOk ) {
        status = f.Write("", 0, NULL);
        if ( status == osal::File::EStatusOk ) {
            status = f.Close();
        }
    }
    //
    return status;
}

osal::posix::File::Status osal::posix::File::FindRecursive(const char* a_name, std::set<std::string> a_patterns, FindCallback* a_callback)
{

    char* dir_name = a_name != NULL && strlen(a_name) > 0 ? strdup(a_name) : NULL;
    if ( dir_name == NULL ) {
        return osal::posix::File::EStatusDoesNotExist;
    }

    size_t last_char_index = strlen(dir_name) - 1;
    if ( dir_name[last_char_index] == '/' ) {
        dir_name[last_char_index] = '\0';
    }

    DIR *  dir      = opendir(dir_name);

    osal::posix::File::Status rv = osal::posix::File::EStatusDoesNotExist;

    DEBUGTRACE("osal_file", "FindRecursive: Open '%s'... %s", dir_name, dir != NULL ? "OK" : "FAILED");

    if ( dir != NULL ) {

        size_t path_len = strlen(dir_name);

        struct dirent *p;

        rv = osal::posix::File::EStatusOk;

        while ( rv == osal::posix::File::EStatusOk && ( p = readdir(dir) ) )
        {
            osal::posix::File::Status rv2 = osal::posix::File::EStatusOk;
            char *buf;
            size_t len;

            /* skip the names "." and ".." as we don't want to recurse on them. */
            if ( strcmp(p->d_name, ".") == 0 || strcmp(p->d_name, "..") == 0 ) {
                continue;
            }

            len = path_len + strlen(p->d_name) + 2;
            buf = (char*)malloc(len);

            if ( buf != NULL ) {

                struct stat statbuf;
                snprintf(buf, len, "%s/%s", dir_name, p->d_name);

                DEBUGTRACE("osal_file", "FindRecursive: '%s'...", buf);

                if ( ! stat(buf, &statbuf) ) {
                    if ( S_ISDIR(statbuf.st_mode) ) {
                        rv2 = FindRecursive(buf, a_patterns, a_callback);
                    } else {
                        for ( std::set<std::string>::iterator it = a_patterns.begin(); it != a_patterns.end() ; ++it ) {
                            if ( fnmatch((*it).c_str(), p->d_name, FNM_NOESCAPE|FNM_CASEFOLD) == 0 ) {
                                char tmp [osal::Dir::kMaxPath] =  {0, 0};
                                snprintf(tmp, sizeof(tmp), "%s/", dir_name);
                                rv2 = a_callback->OnNewFileEntry(tmp, p->d_name) == true ? osal::posix::File::EStatusOk : osal::posix::File::EStatusOpenError;
                                break;
                            }
                        }
                    }
                }

                DEBUGTRACE("osal_file", "FindRecursive: '%s'... %s", buf, rv2 == osal::posix::File::EStatusOk ? "OK" : "FAILED!");

                free(buf);
            }

            rv = rv2;
        }

        closedir(dir);
    }

    free(dir_name);
    return rv;
}

osal::posix::File::Status osal::posix::File::UniqueFileName (const std::string& a_path, const std::string& a_prefix, const std::string& a_extension, std::string& o_name)
{
    if ( a_prefix.length() > 0 ) {
        o_name = a_path + a_prefix + ".XXXXXX." + a_extension;
    } else {
        o_name = a_path + "XXXXXX." + a_extension;
    }

    char f_template[osal::Dir::kMaxPath] = { 0, 0 };
    memcpy(f_template, o_name.c_str(), o_name.length());
    int  fd = mkstemps(f_template, static_cast<int>(sizeof(char)) * static_cast<int>(a_extension.length() + 1));
    if ( -1 == fd ) {
        return osal::posix::File::EStatusOpenError;
    } else {
        close(fd);
    }

    o_name = f_template;

    return osal::posix::File::EStatusOk;
}

osal::posix::File::Status osal::posix::File::Basename (const char* a_uri, std::string& o_name)
{
    o_name = basename((char*)a_uri);
    return osal::posix::File::EStatusOk;
}
