#pragma once
/**
 * @file windows_worker.h - Simple thread based worker for windows, implementation
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

#include "osal/windows/windows_worker.h"
#include "osal/debug_trace.h"

osal::windows::Worker::Worker (const char* a_name, bool a_high_priority)
{
    startup_err_ = 0;
    name_       = a_name;
    running_    = false;   // Assume idle until the thread is effectively created
    run_worker_ = false;   //

    InitializeCriticalSection(&run_condition_lock_);
    InitializeConditionVariable(&run_condition_);

    /*
    * Set running_ true *before* creating the thread
    */
    running_ = true;
    DWORD id;
    thread_handle_ = CreateThread(NULL, 0, &Worker::WorkerRunLoopWrapperInternal, this, 0, &id);
    if (thread_handle_ == INVALID_HANDLE_VALUE) {
        running_ = false; // in case things went wrong
    }

}

/**
 * @brief Wake-up the worker thread
 * @note No effect if it's already running
 */
void osal::windows::Worker::StartWorkerThread ()
{
    EnterCriticalSection(&run_condition_lock_);
    run_worker_ = true;
    LeaveCriticalSection(&run_condition_lock_);
    WakeConditionVariable(&run_condition_);
}

/**
 * @brief Abort this worker.
 */
void osal::windows::Worker::Abort ()
{
    EnterCriticalSection(&run_condition_lock_);
    while (run_worker_ == true) {
        SleepConditionVariableCS(&run_condition_, &run_condition_lock_, INFINITE);
    }
    LeaveCriticalSection(&run_condition_lock_);
}

/**
 * @brief The worker loop, continously sleep and wakes to perform the worker function
 *
 * @return always 0
 */
int osal::windows::Worker::WorkerRunLoop ()
{
   THREADNAME_INFO info;
   info.dwType = 0x1000;
   info.szName = name_;
   info.dwThreadID = GetCurrentThreadId();
   info.dwFlags = 0;

   __try {
      RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
   } __except(EXCEPTION_EXECUTE_HANDLER) {

   }

    while ( running_ == true ) {
        DEBUGTRACE("Worker", "== worker sleeping\n");
        EnterCriticalSection(&run_condition_lock_);
        while (run_worker_ == false) {
            SleepConditionVariableCS(&run_condition_, &run_condition_lock_, INFINITE);
        }
        if ( running_ == true) {
            LeaveCriticalSection(&run_condition_lock_);

            WorkerFunction();

            EnterCriticalSection(&run_condition_lock_);
            run_worker_ = false;
            LeaveCriticalSection(&run_condition_lock_);

            WakeAllConditionVariable(&run_condition_);
        } else {
            run_worker_ = false;
            LeaveCriticalSection(&run_condition_lock_);
        }
    }

    return 0;
}


/**
 * @brief Thread main loop function, just a static C wrapper to
 * call #WorkerRunLoop() that is a civilized class member
 *
 * @param a_self Pointer to the stream instance
 */
void* osal::windows::Worker::WorkerRunLoopWrapper (void* a_self)
{
    return (void*) ((Worker*) a_self)->WorkerRunLoop();
}

DWORD __stdcall osal::windows::Worker::WorkerRunLoopWrapperInternal(LPVOID a_self)
{
    return (DWORD)(WorkerRunLoopWrapper(a_self));
}


osal::windows::Worker::~Worker ()
{
    if (thread_handle_ != INVALID_HANDLE_VALUE) {
        EnterCriticalSection(&run_condition_lock_);
        running_ = false;
        run_worker_ = true;
        LeaveCriticalSection(&run_condition_lock_);
        WakeAllConditionVariable(&run_condition_);
        WaitForSingleObject(thread_handle_, INFINITE);
        CloseHandle(thread_handle_);
        thread_handle_ = INVALID_HANDLE_VALUE;
    }

}
