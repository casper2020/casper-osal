#pragma once
/**
 * @file windows_mutex.h -  mutex for windows OS, declatation
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

#ifndef NRS_OSAL_WINDOWS_WINDOWS_MUTEX_H
#define NRS_OSAL_WINDOWS_WINDOWS_MUTEX_H

#include <windows.h>

#define USE_FAST_MUTEX 1

namespace osal {

    namespace windows {

        class Mutex
        {

        protected: // data

            bool            created_;
#if !defined(USE_FAST_MUTEX)
            HANDLE          mutex_handle_;
#else
            //A critical section object provides synchronization similar to that provided by a mutex object,
            //except that a critical section can be used only by the threads of a single process.
            //Event, mutex, and semaphore objects can also be used in a single-process application,
            //but critical section objects provide a slightly faster,
            //more efficient mechanism for mutual-exclusion synchronization (a processor-specific test and set instruction).
            CRITICAL_SECTION    condition_lock_;
#endif
            char*           name_;

        public: // constructor(s) / destructor

            Mutex (const char* a_name);
            virtual ~Mutex ();

        public: // method(s) / function(s) declaration

            bool Lock ();
            bool Unlock ();

        };

    }

}

#endif // NRS_OSAL_WINDOWS_WINDOWS_MUTEX_H