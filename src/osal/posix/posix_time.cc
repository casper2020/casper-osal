/**
 * @file time.cc - time abstraction for posix
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

#include "osal/posix/posix_time.h"

#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <limits>

const int32_t osal::posix::Time::k_invalid_time = -1;
const char* const osal::posix::Time::k_months_  [] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
const char* const osal::posix::Time::k_weekdays_[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

int64_t osal::posix::Time::GetUTC ()
{
    struct timeval tv;
	struct tm      tm;

    if ( gettimeofday(&tv, NULL) == 0 ) {
		if ( gmtime_r(&tv.tv_sec, &tm) == &tm ) {
            return (int64_t)mktime(&tm);
        }
    }
	return 0;
}

int64_t osal::posix::Time::GetTimeOfDayInMS ()
{
    struct timeval tv;
    if ( gettimeofday(&tv, NULL) == 0 ) {
        return (int64_t) ( ( tv.tv_sec * 1000000L + tv.tv_usec ) / 1000 );
    }
	return 0;
}

long osal::posix::Time::GetTimeOfDayInMicroSeconds ()
{
    struct timeval tv;
    if ( gettimeofday(&tv, NULL) == 0 ) {
        return ( tv.tv_sec * 1000000L + tv.tv_usec );
    }
    return 0;
}

bool osal::posix::Time::GetHumanReadableTimeFromUTC (const int64_t a_utc_timestamp, HumanReadableTime& o_hr_time)
{
    time_t utc = a_utc_timestamp;

    struct tm local;
    if ( localtime_r(&utc, &local) == &local ) {
        o_hr_time.seconds_    = (uint8_t)  local.tm_sec  - ( ( local.tm_gmtoff % 3600 ) % 60 );
        o_hr_time.minutes_    = (uint8_t)  local.tm_min  - ( ( local.tm_gmtoff % 3600 ) / 60 );
        o_hr_time.hours_      = uint8_t   ((long)local.tm_hour - ( local.tm_gmtoff / 3600 ));
        o_hr_time.day_        = (uint8_t)  local.tm_mday;
        o_hr_time.weekday_    = (uint8_t)  local.tm_wday;
        o_hr_time.month_      = (uint8_t)  local.tm_mon  + 1;
        o_hr_time.year_       = (uint16_t) local.tm_year + 1900;
        o_hr_time.tz_hours_   = 0;
        o_hr_time.tz_minutes_ = 0;
        o_hr_time.isdst_      = (int8_t)   local.tm_isdst;
        return true;
    } else {
        return false;
    }
}

bool osal::posix::Time::GetUtcHumanReadableTimeFromUTC (const int64_t a_utc_timestamp, HumanReadableTime& o_hr_time)
{
    time_t utc = (time_t) a_utc_timestamp;

    struct tm local;
    if ( gmtime_r(&utc, &local) == &local ) {
        o_hr_time.seconds_    = (uint8_t)  local.tm_sec;
        o_hr_time.minutes_    = (uint8_t)  local.tm_min;
        o_hr_time.hours_      = (uint8_t)  local.tm_hour;
        o_hr_time.day_        = (uint8_t)  local.tm_mday;
        o_hr_time.weekday_    = (uint8_t)  local.tm_wday;
        o_hr_time.month_      = (uint8_t)  local.tm_mon  + 1;
        o_hr_time.year_       = (uint16_t) local.tm_year + 1900;
        o_hr_time.tz_hours_   = 0;
        o_hr_time.tz_minutes_ = 0;
        o_hr_time.isdst_      = (int8_t)   local.tm_isdst;
        return true;
    } else {
        return false;
    }
}

int64_t osal::posix::Time::GetUtcEpochFromHumanReadableTime (HumanReadableTime& a_hr_time)
{
    struct tm tm;

    tm.tm_sec  = a_hr_time.seconds_;     /* seconds (0 - 60)       */
    tm.tm_min  = a_hr_time.minutes_;     /* minutes (0 - 59)       */
    tm.tm_hour = a_hr_time.hours_;       /* hours (0 - 23)         */
    tm.tm_mday = a_hr_time.day_;         /* day of month (1 - 31)  */
    tm.tm_mon  = a_hr_time.month_ - 1;   /* month of year (0 - 11) */
    tm.tm_year = a_hr_time.year_ - 1900; /* year - 1900            */

    time_t epoch = timegm(&tm);
    return (int64_t) epoch;
}

const char* const osal::posix::Time::GetHumanReadableMonth (const uint8_t& a_month)
{
    return osal::posix::Time::k_months_[a_month - 1];
}

uint8_t osal::posix::Time::GetNumericMonth (const char* const a_name)
{
    for ( size_t idx = 0 ; idx < sizeof(k_months_) / sizeof(k_months_[0]) ; ++idx ) {
        if ( 0 == strcasecmp(k_months_[idx], a_name) ) {
            return static_cast<uint8_t>(idx + 1);
        }
    }
    return std::numeric_limits<uint8_t>::max();
}

const char* const osal::posix::Time::GetHumanReadableWeekday (const uint8_t& a_day)
{
    return osal::posix::Time::k_weekdays_[a_day];
}

/**
 * @return The current localtime ISO81601 w/ timezone format.
 */
std::string osal::posix::Time::GetHumanReadableLocalTimeISO8601WithTZ ()
{
    HumanReadableTime now;
    if ( true == GetHumanReadableLocalTimeFrom(now) ) {
        return ToHumanReadableTimeISO8601WithTZ(now);
    } else {
        return "YYYY-MM-DDTHH:MM:SS+HH:MM";
    }
}

/**
 * @brief Retrieve the current localtime w/ timezone.
 */
bool osal::posix::Time::GetHumanReadableLocalTimeFrom (HumanReadableTime& o_hr_time)
{
    time_t now = (time_t) 0;
    time(&now);
    struct tm local;
    if ( localtime_r(&now, &local) == &local ) {
        o_hr_time.seconds_    = (uint8_t)  local.tm_sec  - ( ( local.tm_gmtoff % 3600 ) % 60 );
        o_hr_time.minutes_    = (uint8_t)  local.tm_min  - ( ( local.tm_gmtoff % 3600 ) / 60 );
        o_hr_time.hours_      = (uint8_t)  local.tm_hour;
        o_hr_time.day_        = (uint8_t)  local.tm_mday;
        o_hr_time.weekday_    = (uint8_t)  local.tm_wday;
        o_hr_time.month_      = (uint8_t)  local.tm_mon  + 1;
        o_hr_time.year_       = (uint16_t) local.tm_year + 1900;
        o_hr_time.tz_hours_   = (uint8_t) (local.tm_gmtoff / 3600);
        o_hr_time.tz_minutes_ = (uint8_t) ((local.tm_gmtoff % 3600) / 60);
        o_hr_time.isdst_      = (int8_t)  (local.tm_isdst);
        return true;
    } else {
        return false;
    }
}

/**
 * @brief Retrieve the current localtime w/ timezone offsetted by n seconds.
 *
 * @param a_seconds
 */
bool osal::posix::Time::GetHumanReadableLocalTimeOffsetBy (const int64_t a_seconds, HumanReadableTime& o_hr_time)
{
    if ( false == GetHumanReadableLocalTimeFrom(o_hr_time) ) {
        return false;
    }
    
    if ( 0 == a_seconds ) {
        return true;
    }
    
    struct tm tm;
    
    tm.tm_sec    = o_hr_time.seconds_;     /* seconds (0 - 60)       */
    tm.tm_min    = o_hr_time.minutes_;     /* minutes (0 - 59)       */
    tm.tm_hour   = o_hr_time.hours_;       /* hours (0 - 23)         */
    tm.tm_mday   = o_hr_time.day_;         /* day of month (1 - 31)  */
    tm.tm_mon    = o_hr_time.month_ - 1;   /* month of year (0 - 11) */
    tm.tm_year   = o_hr_time.year_ - 1900; /* year - 1900            */
    /* offset from CUT in seconds */
    tm.tm_gmtoff = ( o_hr_time.tz_hours_ * 60 * 60 ) + ( o_hr_time.tz_minutes_ * 60 );
    /* Daylight Savings Time flag */
    tm.tm_isdst  = o_hr_time.isdst_;
    /* timezone abbreviation */
    tm.tm_zone   = nullptr;
    
    time_t time = timegm(&tm) + (time_t)a_seconds;
    
    struct tm local;
    if ( &local != gmtime_r(&time, &local) ) {
        return false;
    }
    
    o_hr_time.seconds_    = (uint8_t)  local.tm_sec;
    o_hr_time.minutes_    = (uint8_t)  local.tm_min;
    o_hr_time.hours_      = (uint8_t)  local.tm_hour;
    o_hr_time.day_        = (uint8_t)  local.tm_mday;
    o_hr_time.weekday_    = (uint8_t)  local.tm_wday;
    o_hr_time.month_      = (uint8_t)  local.tm_mon  + 1;
    o_hr_time.year_       = (uint16_t) local.tm_year + 1900;
    
    return true;
}

/**
 * @brief Convert an epoch date to ISO8601 date and time combined format in UTC.
 *        YYYY-MM-DDTHH:MM:SS+HH:MM
 *
 * @return
 */
std::string osal::posix::Time::ToHumanReadableTimeISO8601WithTZ (const HumanReadableTime& a_hr_time)
{
    char buff[27] = {0};
    const int w = snprintf(buff, 26, "%04d-%02d-%02dT%02d:%02d:%02d+%02d:%02d",
                           static_cast<int>(a_hr_time.year_ ), static_cast<int>(a_hr_time.month_  ), static_cast<int>(a_hr_time.day_    ),
                           static_cast<int>(a_hr_time.hours_), static_cast<int>(a_hr_time.minutes_), static_cast<int>(a_hr_time.seconds_),
                           static_cast<int>(a_hr_time.tz_hours_), static_cast<int>(a_hr_time.tz_minutes_)
    );
    return w > 0 ? std::string(buff, w) : "";
}

/**
 * @brief Convert an epoch date to ISO8601 date and time combined format in UTC.
 *        YYYY-MM-DDTHH:MM:SSZ
 *
 * @return
 */
std::string osal::posix::Time::ToHumanReadableTimeISO8601 (const int64_t a_timestamp)
{
    char buff[27] = {0};
    
    osal::posix::Time::HumanReadableTime hr_time;
    
    if ( GetUtcHumanReadableTimeFromUTC(a_timestamp, hr_time) ) {
        const int w = snprintf(buff, 26, "%04d-%02d-%02dT%02d:%02d:%02dZ",
                               static_cast<int>(hr_time.year_ ), static_cast<int>(hr_time.month_  ), static_cast<int>(hr_time.day_    ),
                               static_cast<int>(hr_time.hours_), static_cast<int>(hr_time.minutes_), static_cast<int>(hr_time.seconds_)
        );
        return w > 0 ? std::string(buff, w) : "";
    } else {
        return "";
    }
}
