/**
 * @file posix_dir.cc - posix directory handing, implementation
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

#include "osal/posix/posix_dir.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef __APPLE__
    #include <sys/vfs.h>    /* or <sys/statfs.h> */
#endif
#include <errno.h>
#ifndef _WIN32
#include <dirent.h>
#include <fnmatch.h>
#include <unistd.h>
#else
#include <direct.h>
#endif
#include <sstream>

#if defined(ANDROID)
    #include <sys/statfs.h>
#else
    #include <sys/mount.h>
#endif

#include "osal/osalite.h"
#include "osal/debug_trace.h"

osal::posix::Dir::Status osal::posix::Dir::Exists (const char* a_name)
{
    struct stat st;
    memset(&st, 0, sizeof(struct stat));
    if ( stat( (char *) a_name, &st) == 0 ) {
        if (  ( st.st_mode & S_IFDIR ) == S_IFDIR ) {
            return osal::posix::Dir::EStatusOk;
        } else {
            return osal::posix::Dir::EStatusDoesNotExist;
        }
    }
    return osal::posix::Dir::EStatusDoesNotExist;
}

osal::posix::Dir::Status osal::posix::Dir::CreatePath (const char* a_path)
{
    char*    path      = (char*)a_path;
    char*    tmp       = NULL;
    char*    full_path  = NULL;
    bool     bContinue = true;
    size_t   k         = 0;

    // log
    DEBUGTRACE("create_path", "%-96s", a_path);

    // nothing to create?
    if ( a_path == NULL ) {
        return osal::posix::Dir::EStatusOk;
    }
    // already exists?
    if ( osal::posix::Dir::Exists(a_path) == osal::posix::Dir::EStatusOk ) {
        // yes
        return osal::posix::Dir::EStatusOk;
    }
    // try to create
    if ( osal::posix::Dir::CreateDir(a_path) == osal::posix::Dir::EStatusOk ) {
        // succeeded
        return osal::posix::Dir::EStatusOk;
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
            strcat(full_path,"/");
            while ( bContinue == true && path != NULL && strlen(path)>0 ) {
                // next...
                *tmp = 0;
                while ( path != NULL && strlen(path)>0 && path[0] != '/' ) {
                    strncat(tmp, path, 1);
                    path++;
                }
                if ( path != NULL && strlen(path) > 0 && path[0] == '/' ) {
                    path++;
                }
                //create the path
                if ( tmp != NULL && strlen(tmp) > 0 && tmp[0] != '/' ) {
                    strcat(full_path,tmp);
                    if ( osal::posix::Dir::Exists(full_path) == osal::posix::Dir::EStatusDoesNotExist ) {
                        bContinue = osal::posix::Dir::CreateDir(full_path) == osal::posix::Dir::EStatusOk;
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
    return osal::posix::Dir::Exists(a_path);
}

osal::posix::Dir::Status osal::posix::Dir::CreateDir (const char* a_name)
{
#ifdef _WIN32
    if ( _mkdir(a_name) == -1 ) {
#else
#if defined(DEBUG) || defined(ENABLE_DEBUG)
        mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IXOTH | S_IROTH;
#else
        mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IXOTH;
#endif
    if ( mkdir(a_name, mode) == -1 ) {
#endif
        const int error_number = errno;
        if ( error_number == EEXIST ) {
            DEBUGTRACE("create_dir", "'%-96s' : %s", a_name, "already exists");
            return osal::posix::Dir::EStatusOk;
        } else {
            DEBUGTRACE("create_dir", "'%-96s' : %s ( %d ) ", a_name, "failed", error_number);
            return osal::posix::Dir::EStatusCreateFailed;
        }
    }
    DEBUGTRACE("create_dir", "'%-96s' : %s", a_name, "ok");
    return osal::posix::Dir::EStatusOk;
}

osal::posix::Dir::Status osal::posix::Dir::Find (const char* a_name, const char* a_pattern,
						 std::vector<std::string>* o_results, volatile bool& /* a_abort */)
{
/*    DIR* handle = opendir(a_name);
    //
    if ( handle == NULL ) {
        if ( Exists(a_name) == osal::posix::Dir::EStatusDoesNotExist ) {
            return osal::posix::Dir::EStatusDoesNotExist;
        } else {
            return osal::posix::Dir::EStatusOpenError;
        }
    }
    //
    struct dirent* entry;
    while ( ( entry = readdir(handle) ) != NULL && a_abort == false ) {
        // test type
        if ( entry->d_type & DT_REG ) {
            // file <- it's a match?
            if ( fnmatch(a_pattern, entry->d_name, FNM_CASEFOLD) == 0 ) {
                std::string file_name = a_name;
                file_name += entry->d_name;
                o_results->push_back(file_name);
            }
        } else if ( entry->d_type & DT_DIR ) {
            // dir - skip
        }
    }
    //
    closedir(handle);
    //*/
    OSAL_UNUSED_PARAM(a_name);
    OSAL_UNUSED_PARAM(a_pattern);
    OSAL_UNUSED_PARAM(o_results);
    return osal::posix::Dir::EStatusOk;
}
    
osal::posix::Dir::Status osal::posix::Dir::FindFile (const char* a_dir, const char* a_pattern,
                                                         const std::function<bool(const std::string& a_uri)> a_callback)
{
    DIR* handle = opendir(a_dir);
    if ( handle == NULL ) {
        if ( Exists(a_dir) == osal::posix::Dir::EStatusDoesNotExist ) {
            return osal::posix::Dir::EStatusDoesNotExist;
        } else {
            return osal::posix::Dir::EStatusOpenError;
        }
    }
    std::stringstream ss;
    struct dirent* entry;
    while ( ( entry = readdir(handle) ) != NULL ) {
        // ... is it a file?
        if ( entry->d_type & DT_REG ) {
            // .. and the pattern matches?
            if ( fnmatch(a_pattern, entry->d_name, FNM_CASEFOLD) == 0 ) {
                ss.str("");
                ss << a_dir << entry->d_name;
                if ( false == a_callback(ss.str()) ) {
                    break;
                }
            }
        }
    }
    // ... close handle ...
    closedir(handle);
    // ... we're done ..
    return osal::posix::Dir::EStatusOk;
}
    
osal::posix::Dir::Status osal::posix::Dir::Delete(const char* a_name)
{

    char* dir_name = a_name != NULL && strlen(a_name) > 0 ? strdup(a_name) : NULL;
    if ( dir_name == NULL ) {
        return osal::posix::Dir::EStatusDoesNotExist;
    }

    size_t last_char_index = strlen(dir_name) - 1;
    if ( dir_name[last_char_index] == '/' ) {
        dir_name[last_char_index] = '\0';
    }

    DIR *  dir      = opendir(dir_name);

    osal::posix::Dir::Status rv = osal::posix::Dir::EStatusDoesNotExist;

    DEBUGTRACE("osal_dir", "Delete: Open '%s'... %s", dir_name, dir != NULL ? "OK" : "FAILED");

    if ( dir != NULL ) {

        size_t path_len = strlen(dir_name);

        struct dirent *p;

        rv = osal::posix::Dir::EStatusOk;

        while ( rv == osal::posix::Dir::EStatusOk && ( p = readdir(dir) ) )
        {
            osal::posix::Dir::Status rv2 = osal::posix::Dir::EStatusOk;
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

                DEBUGTRACE("osal_dir", "Delete: '%s'...", buf);

                if ( ! stat(buf, &statbuf) ) {
                    if ( S_ISDIR(statbuf.st_mode) ) {
                        rv2 = Delete(buf);
                    } else {
                        rv2 = unlink(buf) == 0 ? osal::posix::Dir::EStatusOk : osal::posix::Dir::EStatusOpenError;
                    }
                }

                DEBUGTRACE("osal_dir", "Delete: '%s'... %s", buf, rv2 == osal::posix::Dir::EStatusOk ? "OK" : "FAILED!");

                free(buf);
            }

            rv = rv2;
        }

        closedir(dir);
    }

    if ( rv == osal::posix::Dir::EStatusOk ) {
        rv = rmdir(dir_name) == 0 ? osal::posix::Dir::EStatusOk : osal::posix::Dir::EStatusOpenError;
    }

    free(dir_name);

    return rv;
}

osal::posix::Dir::Status osal::posix::Dir::FreeSpace (const char *a_name, int64_t* o_space)
{
    struct statfs stat_data;
    //
    (*o_space) = 0;
    //
    if ( statfs(a_name, &stat_data) == 0 ) {
        //
        (*o_space) = (int64_t)((uint64_t)stat_data.f_bfree * (uint64_t)stat_data.f_bsize);
    } else {
        // failure
        return osal::posix::Dir::EStatusOpenError;
    }
    //
    return osal::posix::Dir::EStatusOk;
}

osal::posix::Dir::Status osal::posix::Dir::TotalSpace (const char *a_name, int64_t* o_space)
{
    struct statfs stat_data;
    //
    (*o_space) = 0;
    //
    if ( statfs(a_name, &stat_data) == 0 ) {
        //
        (*o_space) = (int64_t)((uint64_t)stat_data.f_blocks * (uint64_t)stat_data.f_bsize);
    } else {
        // failure
        return osal::posix::Dir::EStatusOpenError;
    }
    //
    return osal::posix::Dir::EStatusOk;
}
