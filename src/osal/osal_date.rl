/**
 * @file osal_date.rl - ISO-8601 ( subset ) date.
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

#include "osal/osal_date.h"
#include "osal/osal_time.h"
#include "osal/osal_types.h"
#include "osal/osalite.h"
#include <math.h> // NAN
#include <limits> // std::numeric_limits
#include <vector>
#include <assert.h>
#include <cmath> // std::round

const double      osal::Date::k_seconds_per_day_                         = 24.0 * 60.0 * 60.0;
const double      osal::Date::k_excel_time_offset_                       = 25569.0;
const char* const osal::Date::k_default_iso8601_date_format_             = "%04d-%02d-%02d";                          // YYYY-MM-DD
const char* const osal::Date::k_default_iso8601_combined_in_utc_format_  = "%04d-%02d-%02dT%02d:%02d:%02dZ";          // YYYY-MM-DDTHH:MM:SSZ

/**
 * Default constructor.
 */
osal::Date::Date ()
{
    /* empty */
}

/**
 * @brief Destructor.
 */
osal::Date::~Date ()
{
    /* empty */
}

#ifdef __APPLE_
#pragma mark -
#endif

/**
 * @brief Parse a 'date' from a string to an 'Excel' number representation.
 *
 * @param a_value
 *
 * @return
 */
double osal::Date::ToExcelDate (const std::string& a_value)
{
	osal::Time::HumanReadableTime human_time;
    int      cs;
    char*    p   = (char*) a_value.c_str();
    char*    pe  = p + a_value.size();
    double   rv  = NAN;

    human_time.seconds_ = 0;
    human_time.minutes_ = 0;
    human_time.hours_   = 0;
    human_time.weekday_ = std::numeric_limits<uint8_t>::max();
    
    %%{
        machine excel_date_value;

        action set_year
        {
            human_time.year_ = static_cast<uint16_t>(fpc[-3] - '0') * 1000 + (fpc[-2] - '0') * 100 + (fpc[-1] - '0') * 10 + (fpc[0] - '0');
        }

        action set_month
        {
            human_time.month_ = static_cast<uint8_t>(fpc[-1] - '0') * 10 + (fpc[0] - '0');
        }

        action set_day
        {
            human_time.day_ = static_cast<uint8_t>(fpc[-1] - '0') * 10 + (fpc[0] - '0');
        }

        action set_hours
        {
            human_time.hours_ = static_cast<uint8_t>(fpc[-1] - '0') * 10 + (fpc[0] - '0');
        }

        action set_minutes
        {
            human_time.minutes_ = static_cast<uint8_t>(fpc[-1] - '0') * 10 + (fpc[0] - '0');
        }

        action set_seconds
        {
            human_time.seconds_ = static_cast<uint8_t>(fpc[-1] - '0') * 10 + (fpc[0] - '0');
        }

        action set_tz_hours
        {
            human_time.tz_hours_ = static_cast<uint8_t>(fpc[-1] - '0') * 10 + (fpc[0] - '0');
        }

        action set_tz_minutes
        {
            human_time.tz_minutes_ = static_cast<uint8_t>(fpc[-1] - '0') * 10 + (fpc[0] - '0');
        }

        sep  = '-' | '/';
        year_first_date = [0-9]{4} @set_year sep [0-9]{2} @set_month sep [0-9]{2} @set_day ;
        day_first_date  = [0-9]{2} @set_day  sep [0-9]{2} @set_month sep [0-9]{4} @set_year;

        iso8601_date_combined_in_utc  = [0-9]{4} @set_year sep [0-9]{2} @set_month sep [0-9]{2} @set_day 'T' [0-9]{2} @set_hours ':' [0-9]{2} @set_minutes ':' [0-9]{2} @set_seconds 'Z';
        iso8601_date_combined_with_tz = [0-9]{4} @set_year sep [0-9]{2} @set_month sep [0-9]{2} @set_day 'T' [0-9]{2} @set_hours ':' [0-9]{2} @set_minutes ':' [0-9]{2} @set_seconds '+' [0-9]{2} @set_tz_hours ':' [0-9]{2} @set_tz_minutes;

        main := (year_first_date | day_first_date | iso8601_date_combined_in_utc | iso8601_date_combined_with_tz);

        write data;
        write init;
        write exec;
    }%%

    if ( cs >= excel_date_value_first_final ) {
        rv = osal::Time::GetUtcEpochFromHumanReadableTime(human_time) / osal::Date::k_seconds_per_day_ + osal::Date::k_excel_time_offset_;
    }

    OSAL_UNUSED_PARAM(excel_date_value_error);
    OSAL_UNUSED_PARAM(excel_date_value_en_main);

    return rv;
}

/**
 * @brief Convert date components int to an a 'Excel' date.
 *
 * @param a_year
 * @param a_month
 * @param a_day
 *
 * @return
 */
double osal::Date::ToExcelDate (const uint16_t& a_year, const uint8_t& a_month, const uint8_t& a_day)
{
    osal::Time::HumanReadableTime human_time;

    human_time.seconds_    = 0;
    human_time.minutes_    = 0;
    human_time.hours_      = 0;
    human_time.year_       = a_year;
    human_time.day_        = a_day;
    human_time.weekday_    = std::numeric_limits<uint8_t>::max();
    human_time.month_      = a_month;
    human_time.tz_hours_   = 0;
    human_time.tz_minutes_ = 0;
    human_time.isdst_      = 0;

    return ((static_cast<double>(osal::Time::GetUtcEpochFromHumanReadableTime(human_time)) / osal::Date::k_seconds_per_day_) + osal::Date::k_excel_time_offset_);
}

/**
 * @brief Extract from an 'Excel' date the 'year' component.
 *
 * @param a_date
 *
 * @return
 */
uint16_t osal::Date::YearFromExcelDate (const double& a_date)
{
    osal::Time::HumanReadableTime human_time;
    osal::Time::GetHumanReadableTimeFromUTC(static_cast<int64_t>(ExcelDateToEpoch(a_date)), human_time);
    return human_time.year_;
}

/**
 * @brief Extract from an 'Excel' date the 'month' component.
 *
 * @param a_date
 *
 * @return
 */
uint8_t osal::Date::MonthFromExcelDate (const double& a_date)
{
    osal::Time::HumanReadableTime human_time;
    osal::Time::GetHumanReadableTimeFromUTC(static_cast<int64_t>(ExcelDateToEpoch(a_date)), human_time);
    return human_time.month_;
}

/**
 * @brief Extract from an 'Excel' date the 'day' component.
 *
 * @param a_date
 *
 * @return
 */
uint8_t osal::Date::DayFromExcelDate (const double& a_date)
{
    osal::Time::HumanReadableTime human_time;
    osal::Time::GetHumanReadableTimeFromUTC(static_cast<int64_t>(ExcelDateToEpoch(a_date)), human_time);
    return human_time.day_;
}

/**
 * @brief Extract from an 'Excel' date the 'hours' component.
 *
 * @param a_date
 *
 * @return
 */
uint8_t osal::Date::HoursFromExcelDate (const double& a_date)
{
    osal::Time::HumanReadableTime human_time;
    osal::Time::GetHumanReadableTimeFromUTC(static_cast<int64_t>(ExcelDateToEpoch(a_date)), human_time);
    return human_time.hours_;
}

/**
 * @brief Extract from an 'Excel' date the 'minutes' component.
 *
 * @param a_date
 *
 * @return
 */
uint8_t osal::Date::MinutesFromExcelDate (const double& a_date)
{
    osal::Time::HumanReadableTime human_time;
    osal::Time::GetHumanReadableTimeFromUTC(static_cast<int64_t>(ExcelDateToEpoch(a_date)), human_time);
    return human_time.minutes_;
}

/**
 * @brief Extract from an 'Excel' date the 'seconds' component.
 *
 * @param a_date
 *
 * @return
 */
uint8_t osal::Date::SecondsFromExcelDate (const double& a_date)
{
    osal::Time::HumanReadableTime human_time;
    osal::Time::GetHumanReadableTimeFromUTC(static_cast<int64_t>(ExcelDateToEpoch(a_date)), human_time);
    return human_time.seconds_;
}

/**
 * @brief Convert an 'epoch' value to an a 'Excel' date.
 *
 * @param a_epoch
 *
 * @return
 */
double osal::Date::EpochToExcelDate (const time_t& a_epoch)
{
    return ((static_cast<double>(a_epoch) / osal::Date::k_seconds_per_day_) + osal::Date::k_excel_time_offset_);
}

/**
 * @brief Convert an 'Excel' date to 'epoch'.
 *
 * @param a_date
 *
 * @return
 */
time_t osal::Date::ExcelDateToEpoch (const double& a_date)
{
    return static_cast<time_t>((a_date - osal::Date::k_excel_time_offset_) * osal::Date::k_seconds_per_day_ + 0.5);
}

/**
 * @brief Convert an excel date to ISO8601 date format.
 *        YYYY-MM-DD
 *
 * @param a_date
 *
 * @return
 */
std::string osal::Date::ExcelDateToISO8601 (const double& a_date)
{
    if ( 0.0 == a_date ) {
        return "";
    }
    osal::Time::HumanReadableTime human_time;
    if ( true == osal::Time::GetUtcHumanReadableTimeFromUTC(static_cast<int64_t>(osal::Date::ExcelDateToEpoch(a_date)), human_time) ) {
        const int required_buffer_size = std::snprintf(nullptr, 0,
                                                       osal::Date::k_default_iso8601_date_format_,
                                                       static_cast<int>(human_time.year_), static_cast<int>(human_time.month_), static_cast<int>(human_time.day_)
        );
        std::vector<char> buffer(static_cast<size_t>(required_buffer_size) + 1);
        const int bytes_written = std::snprintf(&buffer[0], buffer.size(),
                                                osal::Date::k_default_iso8601_date_format_,
                                                static_cast<int>(human_time.year_), static_cast<int>(human_time.month_), static_cast<int>(human_time.day_)
        );
        return bytes_written > 1 ? std::string { buffer.begin(), buffer.end() - 1 } : "";
    } else {
        return "";
    }
}

/**
 * @brief Convert an excel date to ISO8601 date and time combined format in UTC.
 *        YYYY-MM-DDTHH:MM:SSZ
 *
 * @param a_date
 *
 * @return
 */
std::string osal::Date::ExcelDateToISO8601CombinedInUTC (const double& a_date)
{
    if ( 0.0 == a_date ) {
        return "";
    }
    osal::Time::HumanReadableTime human_time;
    if ( true == osal::Time::GetUtcHumanReadableTimeFromUTC(static_cast<int64_t>(osal::Date::ExcelDateToEpoch(a_date)), human_time) ) {
        const int required_buffer_size = std::snprintf(nullptr, 0,
                                                       osal::Date::k_default_iso8601_combined_in_utc_format_,
                                                       static_cast<int>(human_time.year_ ), static_cast<int>(human_time.month_  ), static_cast<int>(human_time.day_    ),
                                                       static_cast<int>(human_time.hours_), static_cast<int>(human_time.minutes_), static_cast<int>(human_time.seconds_)
        );
        std::vector<char> buffer(static_cast<size_t>(required_buffer_size) + 1);
        const int bytes_written = std::snprintf(&buffer[0], buffer.size(),
                                                osal::Date::k_default_iso8601_combined_in_utc_format_,
                                                static_cast<int>(human_time.year_ ), static_cast<int>(human_time.month_  ), static_cast<int>(human_time.day_    ),
                                                static_cast<int>(human_time.hours_), static_cast<int>(human_time.minutes_), static_cast<int>(human_time.seconds_)
        );
        return bytes_written > 1 ? std::string { buffer.begin(), buffer.end() - 1 } : "";
    } else {
        return "";
    }
}

/**
 * @brief Convert an epoch date to ISO8601 date and time combined format in UTC.
 *        YYYY-MM-DDTHH:MM:SSZ
 *
 * @param a_date
 *
 * @return
 */
std::string osal::Date::EpochToISO8601CombinedInUTC (const time_t& a_epoch)
{
    osal::Time::HumanReadableTime human_time;
    if ( true == osal::Time::GetUtcHumanReadableTimeFromUTC(static_cast<int64_t>(a_epoch), human_time) ) {
        const int required_buffer_size = std::snprintf(nullptr, 0,
                                                       osal::Date::k_default_iso8601_combined_in_utc_format_,
                                                       static_cast<int>(human_time.year_ ), static_cast<int>(human_time.month_  ), static_cast<int>(human_time.day_    ),
                                                       static_cast<int>(human_time.hours_), static_cast<int>(human_time.minutes_), static_cast<int>(human_time.seconds_)
        );
        std::vector<char> buffer(static_cast<size_t>(required_buffer_size) + 1);
        const int bytes_written = std::snprintf(&buffer[0], buffer.size(),
                                                osal::Date::k_default_iso8601_combined_in_utc_format_,
                                                static_cast<int>(human_time.year_ ), static_cast<int>(human_time.month_  ), static_cast<int>(human_time.day_    ),
                                                static_cast<int>(human_time.hours_), static_cast<int>(human_time.minutes_), static_cast<int>(human_time.seconds_)
        );
        return bytes_written > 1 ? std::string { buffer.begin(), buffer.end() - 1 } : "";
    } else {
        return "";
    }
}

/**
 * @brief Run some tests targeting ISO8601 subset conversion funcions implementation.
 */

void osal::Date::TestISO80601 ()
{
#if defined(DEBUG) || defined(_DEBUG) || defined(ENABLE_DEBUG)
    // ISO8601 -> Excel
    assert(42559.0    == osal::Date::ToExcelDate("2016-07-08")          );
    assert(42559.4078 == std::round(osal::Date::ToExcelDate("2016-07-08T09:47:14Z") * 10000) / 10000);
    assert(42559.4078 == std::round(osal::Date::ToExcelDate("2016-07-08T09:47:14+00:00") * 10000) / 10000);
    // Epoch UTC -> ISO8601 combined UTC format
    const time_t epoch = 1467971234; // Fri, 8 Jul 2016 09:47:14
    assert(0 == osal::Date::EpochToISO8601CombinedInUTC(epoch).compare("2016-07-08T09:47:14Z"));
    // Epoch -> Excel
    const double excel_date = 42559.4078;
    assert(excel_date == std::round(osal::Date::EpochToExcelDate(epoch) * 10000) / 10000);
    // Excel -> ISO8601
    assert(0 == ExcelDateToISO8601(excel_date).compare("2016-07-08"));
    assert(0 == ExcelDateToISO8601CombinedInUTC(excel_date).compare("2016-07-08T09:47:14Z"));
    // YYYY-MM-DD -> Excel
    assert(static_cast<double>(static_cast<int>(excel_date)) == ToExcelDate(/* a_year */2016, /* a_month */ 07, /* a_day */ 8));
    // Date Components from Excel
    assert(2016 == YearFromExcelDate(excel_date));
    assert(7 == MonthFromExcelDate(excel_date));
    assert(8 == DayFromExcelDate(excel_date));
    assert(9 == HoursFromExcelDate(excel_date));
    assert(47 == MinutesFromExcelDate(excel_date));
    assert(14 == SecondsFromExcelDate(excel_date));
#endif
}
