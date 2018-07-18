#pragma once
/**
 * @file windows_time.h
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

#ifndef NRS_OSAL_WINDOWS_WINDOWS_TIME_H
#define NRS_OSAL_WINDOWS_WINDOWS_TIME_H

#include <stdint.h>
#include <windows.h>

namespace osal {
    namespace windows {

        class Time
        {
		public: // constants

			static const __int32 Time::k_invalid_time;
            static const __int64 Time::_onesec_in100ns;

            struct HumanReadableTime {
                uint8_t	 seconds_;  /* seconds after the minute [0-60] */
                uint8_t	 minutes_;	/* minutes after the hour [0-59]   */
                uint8_t  hours_;    /* hours since midnight [0-23]     */
                uint8_t day_;       /* day of the month [1-31]         */
                uint8_t month_;     /* months since January [1-12]     */
                uint16_t year_;     /* years since 1970...2038         */
            };

        public: // method(s) / function(s) declaration

            static int32_t GetUTC ();

			//windows only
            static int32_t GetUTC (const FILETIME&   a_filetime);

            static int64_t GetTimeOfDayInMS ();

            static bool    GetHumanReadableTimeFromUTC (const int32_t a_utc_timestamp, HumanReadableTime* o_hr_time);

        };
    }
}

#endif // NRS_OSAL_WINDOWS_WINDOWS_TIME_H
