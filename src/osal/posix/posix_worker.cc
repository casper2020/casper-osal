/**
 * @file posix_worker.cc - Simple thread based worker, implementation
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

#include "osal/posix/posix_worker.h"
#include "osal/debug_trace.h"

#include "osal/osal_types.h"

#ifdef ANDROID
#include <jni.h>
extern JavaVM* g_java_vm_; // Pointer to the process wide JavaVM from streamer_jni.cc
#define detach_thread_from_java()  if ( g_java_vm_ != NULL ) {  g_java_vm_->DetachCurrentThread(); }
#else
#define detach_thread_from_java()
#endif

osal::posix::Worker::Worker (const char* a_name, bool a_high_priority)
{
    name_        = a_name;
    startup_err_ = 0;
    pthread_mutexattr_t mutexAttr;

    running_    = false;   // Assume idle until the thread is effectively created
    run_worker_ = false;   //

    /*
     * Create run mutexes and conditions, worker will sleep in the condition
     */
    startup_err_ += pthread_mutexattr_init(&mutexAttr);
    startup_err_ += pthread_mutexattr_settype(&mutexAttr, PTHREAD_MUTEX_ERRORCHECK);
    startup_err_ += pthread_mutex_init(&run_mutex_, &mutexAttr);
    startup_err_ += pthread_mutexattr_destroy(&mutexAttr);
    startup_err_ += pthread_cond_init(&run_condition_, NULL);

#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)

    pthread_attr_t attr;

    if ( a_high_priority == true ) {

        pthread_attr_init(&attr);

        int policy = 0;

        startup_err_ += pthread_attr_getschedpolicy(&attr, &policy);
        startup_err_ += pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);

        struct sched_param current_sched_param;
        startup_err_ += pthread_attr_getschedparam(&attr, &current_sched_param);

        struct sched_param new_sched_param;
        new_sched_param.sched_priority = current_sched_param.sched_priority  + 4; // sched_get_priority_max(policy);
        startup_err_ += pthread_attr_setschedparam(&attr, &new_sched_param);
    }
#endif

    if ( startup_err_ == 0 ) {
        /*
         * Set running_ true *before* creating the thread
         */
        running_ = true;
        startup_err_ += pthread_create(&thread_handle_, NULL, &Worker::WorkerRunLoopWrapper, this);
        startup_err_ += pthread_detach(thread_handle_);
        if ( startup_err_ != 0 ) {
            running_ = false; // in case things went wrong
        }
    }

#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
    if ( a_high_priority == true ) {
        pthread_attr_destroy(&attr);
    }
#endif

    OSAL_UNUSED_PARAM(a_high_priority);
}

/**
 * @brief Wake-up the worker thread
 * @note No effect if it's already running
 */
void osal::posix::Worker::StartWorkerThread ()
{
    pthread_mutex_lock(&run_mutex_);
    run_worker_ = true;
    pthread_mutex_unlock(&run_mutex_);
    pthread_cond_broadcast(&run_condition_);
}

/**
 * @brief Abort this worker.
 */
void osal::posix::Worker::Abort ()
{
    pthread_mutex_lock(&run_mutex_);
    while (run_worker_ == true) {
        pthread_cond_wait(&run_condition_, &run_mutex_);
    }
    pthread_mutex_unlock(&run_mutex_);
}

/**
 * @brief The worker loop, continously sleep and wakes to perform the worker function
 *
 * @return always 0
 */
int osal::posix::Worker::WorkerRunLoop ()
{
#ifdef __APPLE__
    pthread_setname_np(name_);
#else
    pthread_setname_np(pthread_self(), name_);
#endif

    while ( running_ == true ) {
        DEBUGTRACE("Worker", "== worker sleeping\n");
        pthread_mutex_lock(&run_mutex_);
        while (run_worker_ == false) {
            pthread_cond_wait(&run_condition_, &run_mutex_);
        }
        pthread_mutex_unlock(&run_mutex_);

        WorkerFunction();

        pthread_mutex_lock(&run_mutex_);
        run_worker_ = false;
        pthread_mutex_unlock(&run_mutex_);
        pthread_cond_broadcast(&run_condition_);
    }

    detach_thread_from_java();
    return 0;
}


/**
 * @brief Thread main loop function, just a static C wrapper to
 * call #WorkerRunLoop() that is a civilized class member
 *
 * @param a_self Pointer to the stream instance
 */
void* osal::posix::Worker::WorkerRunLoopWrapper (void* a_self)
{
	return (void*) (intptr_t)((Worker*) a_self)->WorkerRunLoop();
}

osal::posix::Worker::~Worker ()
{
    // TODO wait for thread exit

    // Safety checks before destruction
    pthread_cond_destroy(&run_condition_);
    pthread_mutex_destroy(&run_mutex_);
}

