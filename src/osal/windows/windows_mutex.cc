/**
 * @file windows_mutex.cc -  mutex for windows OS, implementation
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

#include "osal/windows/windows_mutex.h"
#include "osal/debug_trace.h"

#include <string.h>
#include <stdlib.h>

#if !defined(USE_FAST_MUTEX)
osal::windows::Mutex::Mutex (const char* a_name)
{
    mutex_handle_ = CreateMutex(
        NULL,              // default security attributes
        FALSE,             // initially not owned
        NULL);             // unnamed mutex

    name_ = a_name != NULL ? strdup(a_name) : NULL;

    created_ = mutex_handle_ == INVALID_HANDLE_VALUE;
}

osal::windows::Mutex::~Mutex ()
{
    if ( mutex_handle_ != INVALID_HANDLE_VALUE ) {
        created_ = CloseHandle(mutex_handle_) == 0;
    }

    if ( name_ != NULL ) {
        free(name_);
        name_ = NULL;
    }
}

bool osal::windows::Mutex::Lock ()
{
    DWORD dwWaitResult = WaitForSingleObject(
        mutex_handle_,    // handle to mutex
        INFINITE);  // no time-out interval

    DEBUGTRACE_MUTEX("posix_mutex", "%s->Lock() thread_id=%d\n", name_ != NULL ? name_ : "Mutex", GetCurrentThread() );
    DEBUGIF("posix_mutex_e") {
        if ( dwWaitResult != WAIT_OBJECT_0 ) {
            DEBUGTRACEX("posix_mutex_e", "%p.Lock():%d\n", this, dwWaitResult);
        }
    }
    return dwWaitResult == WAIT_OBJECT_0;
}

bool osal::windows::Mutex::Unlock ()
{
    DEBUGTRACE_MUTEX("posix_mutex", "%s->Unlock() thread_id=%d\n", name_ != NULL ? name_ : "Mutex", GetCurrentThread() );
    bool rv = ReleaseMutex(mutex_handle_) != 0;

    DEBUGIF("posix_mutex_e") {
        if ( rv == false ) {
            DEBUGTRACEX("posix_mutex_e", "%p.Unlock():%d\n", this, GetLastError() );
        }
    }
    return rv;
}

#else

osal::windows::Mutex::Mutex (const char* a_name)
{
    InitializeCriticalSection(&condition_lock_);

    name_ = a_name != NULL ? strdup(a_name) : NULL;

    created_ = true;
}

osal::windows::Mutex::~Mutex ()
{
    created_ = false;

    if ( name_ != NULL ) {
        free(name_);
        name_ = NULL;
    }
}

bool osal::windows::Mutex::Lock ()
{
    EnterCriticalSection(&condition_lock_);
    DEBUGTRACE_MUTEX("posix_mutex", "%s->Lock() thread_id=%d\n", name_ != NULL ? name_ : "Mutex", GetCurrentThread() );
    return true;
}

bool osal::windows::Mutex::Unlock ()
{
    DEBUGTRACE_MUTEX("posix_mutex", "%s->Unlock() thread_id=%d\n", name_ != NULL ? name_ : "Mutex", GetCurrentThread() );
    LeaveCriticalSection(&condition_lock_);
    return true;
}


#endif

// endof $Id: $
