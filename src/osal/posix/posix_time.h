#pragma once
/**
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

#ifndef NRS_OSAL_POSIX_TIME_H
#define NRS_OSAL_POSIX_TIME_H

#include <stdint.h>
#include <string>

namespace osal {

    namespace posix {

        class Time
        {
        public: // Helper struct for decomposed time

            struct HumanReadableTime {
                uint8_t	 seconds_;      /* seconds after the minute [0-60] */
                uint8_t	 minutes_;      /* minutes after the hour [0-59]   */
                uint8_t  hours_;        /* hours since midnight [0-23]     */
                uint8_t  day_;          /* day of the month [1-31]         */
                uint8_t  weekday_;      /* weekday 0 - Sunday, ...         */
                uint8_t  month_;        /* months since January [1-12]     */
                uint16_t year_;         /* years since 1970...2038         */
                uint8_t  tz_hours_;     /* offset from UTC in hours        */
                uint8_t  tz_minutes_;   /* offset from UTC in minutes      */
                int8_t   isdst_;        /* Daylight Savings Time flag      */
            };

        public: // static data

            static const int32_t k_invalid_time;
            static const char* const k_months_[];
            static const char* const k_weekdays_[];

        public: // method(s) / function(s) declaration

            static int64_t           GetUTC                             ();
            static int64_t           GetTimeOfDayInMS                   ();
            static long              GetTimeOfDayInMicroSeconds         ();

            static bool              GetHumanReadableTimeFromUTC            (const int64_t a_utc_timestamp, HumanReadableTime& o_hr_time);
            static bool              GetUtcHumanReadableTimeFromUTC         (const int64_t a_utc_timestamp, HumanReadableTime& o_hr_time);
            static int64_t           GetUtcEpochFromHumanReadableTime       (HumanReadableTime& a_hr_time);
            static const char* const GetHumanReadableMonth                  (const uint8_t& a_month);
            static uint8_t           GetNumericMonth                        (const char* const a_name);
            static const char* const GetHumanReadableWeekday                (const uint8_t& a_weekday);
            static bool              GetHumanReadableLocalTimeFrom          (HumanReadableTime& o_hr_time);
            static bool              GetHumanReadableLocalTimeOffsetBy      (const int64_t a_seconds, HumanReadableTime& o_hr_time);
            static std::string       GetHumanReadableLocalTimeISO8601WithTZ ();
            static std::string       ToHumanReadableTimeISO8601WithTZ       (const HumanReadableTime& a_hr_time);
            static std::string       ToHumanReadableTimeISO8601             (const int64_t a_timestamp);
            

        };

    }
}

#endif // NRS_OSAL_POSIX_TIME_H
