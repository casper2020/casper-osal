/**
 * @file posix_condition_variable.cc - posix condition variable implementation
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

#include "osal/posix/posix_condition_variable.h"

#ifndef _WIN32
#include <sys/time.h>
#endif
#include <errno.h>

osal::posix::ConditionVariable::ConditionVariable ()
{
    pthread_mutexattr_t mutex_attr;

    pthread_mutexattr_init(&mutex_attr);
    pthread_mutex_init(&condition_lock_, &mutex_attr);
    pthread_mutexattr_destroy(&mutex_attr);
    pthread_cond_init(&condition_, NULL);
    count_ = 0;
}

osal::posix::ConditionVariable::~ConditionVariable ()
{
    pthread_cond_destroy(&condition_);
    pthread_mutex_destroy(&condition_lock_);
}

void osal::posix::ConditionVariable::Wait ()
{
    pthread_mutex_lock(&condition_lock_);
    while ( count_ == 0 ) {
        pthread_cond_wait(&condition_, &condition_lock_);
    }
    count_ = count_ - 1;
    pthread_mutex_unlock(&condition_lock_);
}

#ifndef _WIN32 // MARTELADA_TO_REMOVE_WIN32
void osal::posix::ConditionVariable::Wait (const int64_t a_timeout_seconds)
{
    struct timeval  now;
    struct timespec timeout;
    //
    gettimeofday(&now, NULL);
    timeout.tv_sec = now.tv_sec + a_timeout_seconds;
    timeout.tv_nsec = now.tv_usec * 1000;
    //
    pthread_mutex_lock(&condition_lock_);
    int rv  = 0;
    while ( count_ == 0 &&  rv != ETIMEDOUT ) {
        rv = pthread_cond_timedwait(&condition_, &condition_lock_, &timeout);
    }
    if ( rv != ETIMEDOUT ) {
        count_ = count_ - 1;
    }
    pthread_mutex_unlock(&condition_lock_);
}
#endif

void osal::posix::ConditionVariable::Wake ()
{
    pthread_mutex_lock(&condition_lock_);
    if ( count_ == 0 ) {
        pthread_cond_signal(&condition_);
    }
    count_ = count_ + 1;
    pthread_mutex_unlock(&condition_lock_);
}

// endof $Id: $
