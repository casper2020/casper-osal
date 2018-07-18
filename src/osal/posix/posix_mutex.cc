/**
 * @file mutex.cc - posix mutext wrapper.
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

#include "osal/posix/posix_mutex.h"
#include "osal/debug_trace.h"

#include <string.h>
#include <stdlib.h>

osal::posix::Mutex::Mutex (const char* a_name)
{
    int rv;
    pthread_mutexattr_t mutex_attr;

    rv  = pthread_mutexattr_init(&mutex_attr);
    rv += pthread_mutex_init(&mutex_, &mutex_attr);
    rv += pthread_mutexattr_destroy(&mutex_attr);

    name_ = a_name != NULL ? strdup(a_name) : NULL;

    created_ = rv == 0;
}

osal::posix::Mutex::~Mutex ()
{
    created_ = pthread_mutex_destroy(&mutex_) == 0 ? false : created_;
    if ( name_ != NULL ) {
        free(name_);
        name_ = NULL;
    }
}

bool osal::posix::Mutex::Lock ()
{
    int rv = pthread_mutex_lock(&mutex_);
    DEBUGTRACE_MUTEX("posix_mutex", "%s->Lock()", name_ != NULL ? name_ : "Mutex");
    DEBUGIF("posix_mutex_e") {
        if ( rv != 0 ) {
            DEBUGTRACEX("posix_mutex_e", "%p.Lock():%d\n", this, rv);
        }
    }
    return rv == 0;
}

bool osal::posix::Mutex::Unlock ()
{
    DEBUGTRACE_MUTEX("posix_mutex", "%s->Unlock()", name_ != NULL ? name_ : "Mutex");
    int rv = pthread_mutex_unlock(&mutex_);
    DEBUGIF("posix_mutex_e") {
        if ( rv != 0 ) {
            DEBUGTRACEX("posix_mutex_e", "%p.Unlock():%d\n", this, rv);
        }
    }
    return rv == 0;
}

// endof $Id: $
