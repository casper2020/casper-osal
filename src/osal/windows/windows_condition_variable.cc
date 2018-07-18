/**
 * @file windows_condition_variable.cc - condition variable windows implementation
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

#include "osal/windows/windows_condition_variable.h"

osal::windows::ConditionVariable::ConditionVariable ()
{
    InitializeCriticalSection(&condition_lock_);
    InitializeConditionVariable(&condition_);
    count_ = 0;
}

osal::windows::ConditionVariable::~ConditionVariable ()
{

}

void osal::windows::ConditionVariable::Wait (int32_t a_timeout_in_secs)
{
    BOOL rv = FALSE;
    BOOL timeout = FALSE;

    EnterCriticalSection(&condition_lock_);
    while ( count_ == 0 && timeout == FALSE ) {
        rv = SleepConditionVariableCS(&condition_, &condition_lock_, a_timeout_in_secs * 1000);
        timeout = (rv == FALSE && GetLastError() == ERROR_TIMEOUT);
    }
    if ( timeout == FALSE ) {
        count_ = count_ - 1;
    }
    LeaveCriticalSection(&condition_lock_);
}

void osal::windows::ConditionVariable::Wait ()
{
    EnterCriticalSection(&condition_lock_);
    while ( count_ == 0 ) {
        SleepConditionVariableCS(&condition_, &condition_lock_, INFINITE);
    }
    count_ = count_ - 1;
    LeaveCriticalSection(&condition_lock_);
}

void osal::windows::ConditionVariable::Wake ()
{
    EnterCriticalSection(&condition_lock_);
    if ( count_ == 0 ) {
        WakeConditionVariable(&condition_);
    }
    count_ = count_ + 1;
    LeaveCriticalSection(&condition_lock_);
}

// endof $Id: $
