/**
 * @file windows_time.cc
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

#include "osal/windows/windows_time.h"
#include <time.h>

const __int32 osal::windows::Time::k_invalid_time = (__int32) 10000000;

const __int64 osal::windows::Time::_onesec_in100ns = (__int64) 10000000;

int32_t osal::windows::Time::GetUTC (const FILETIME&   a_filetime)
{
    __int64 value = 0;
    //
    // 64 -> 32 bit
    //
    // A FILETIME is a 64-bit for the number of 100-nanosecond periods since January 1, 1601.
    //
    value = ( (__int64)(a_filetime.dwHighDateTime) << 32) | a_filetime.dwLowDateTime;

    //
    // The time_t is a 32-bit value for the number of seconds since January 1, 1970.
    // Convert by subtracting the number of 100-nanosecond period between 01-01-1970
    // and 01-01-1601, from time_t the divide by 1e+7 to get to the same base granularity.
    //
    return (int32_t) ( ( value - 116444736000000000ui64 ) / 10000000ui64 );
}

int32_t osal::windows::Time::GetUTC ()
{
    SYSTEMTIME systime;
    FILETIME   filetime;


    // Retrieves the current system date and time. The system time is expressed in Coordinated Universal Time (UTC).
    // http://msdn.microsoft.com/en-us/library/ms724390(VS.85).aspx
    GetSystemTime(&systime);

    // Converts a system time to file time format. System time is based on Coordinated Universal Time (UTC).
    // http://msdn.microsoft.com/en-us/library/ms724948(VS.85).aspx
    SystemTimeToFileTime(&systime, &filetime);

    return GetUTC(filetime);
}

bool osal::windows::Time::GetHumanReadableTimeFromUTC (const int32_t a_utc_timestamp, HumanReadableTime* o_hr_time)
{
    // @TODO WINDOWS PORT
    return false;
}

