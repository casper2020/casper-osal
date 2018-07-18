/**
 * @file osal_date.h - ISO-8601 ( subset ) date parser
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
 * casper-osal  is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with osal.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#ifndef NRS_OSAL_DATE_H_
#define NRS_OSAL_DATE_H_

#include <string>

namespace osal
{

    class Date
    {

    public: // Static Const Data

        static const double      k_seconds_per_day_;
        static const double      k_excel_time_offset_;

    private: // Static Const Data

        static const char* const k_default_iso8601_date_format_;
        static const char* const k_default_iso8601_combined_in_utc_format_;

    public: // Constructor(s) / Destructor

        Date ();
        virtual ~Date ();

    public: // Method(s) / Function(s)

        static double      ToExcelDate                     (const std::string& a_date);
        static double      ToExcelDate                     (const uint16_t& a_year, const uint8_t& a_month, const uint8_t& a_day);
        static uint16_t    YearFromExcelDate               (const double& a_date);
        static uint8_t     MonthFromExcelDate              (const double& a_date);
        static uint8_t     DayFromExcelDate                (const double& a_date);
        static uint8_t     HoursFromExcelDate              (const double& a_date);
        static uint8_t     MinutesFromExcelDate            (const double& a_date);
        static uint8_t     SecondsFromExcelDate            (const double& a_date);
        static double      EpochToExcelDate                (const time_t& a_epoch);
        static time_t      ExcelDateToEpoch                (const double& a_date);
        static std::string ExcelDateToISO8601              (const double& a_date);
        static std::string ExcelDateToISO8601CombinedInUTC (const double& a_date);
        static std::string EpochToISO8601CombinedInUTC     (const time_t& a_epoch);

        static void        TestISO80601                    ();

    }; // end of class DateParser

} // end of namespace osal

#endif // NRS_OSAL_DATE_H_
