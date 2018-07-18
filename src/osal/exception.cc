/**
 * @file exception.cc Implementation of Exception class that carries a user message and
 *       the location of the fault origin
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

#include "osal/exception.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Exception constructor
 *
 * @param a_code
 * @param a_file Should be filled with __FILE__
 * @param a_line Should be filled with __LINE__
 * @param a_format  printf like format followed by a variable number of arguments
 */
osal::Exception::Exception (const char* const a_code, const char* a_file, int a_line, bool a_is_null, const char* a_format, ...) throw()
{
    is_null_ = a_is_null;
    line_    = a_line;
    file_    = a_file;
    code_    = a_code;
    va_list args;
    va_start(args, a_format);
    vsnprintf(message_, sizeof(message_), a_format, args);
    va_end(args);
}

/**
 * @brief Exception constructor
 *
 * @param a_file Should be filled with __FILE__
 * @param a_line Should be filled with __LINE__
 * @param a_format  printf like format followed by a variable number of arguments
 */
osal::Exception::Exception (const char* a_file, int a_line, bool a_is_null, const char* a_format, ...) throw()
{
    is_null_ = a_is_null;
    line_    = a_line;
    file_    = a_file;
    va_list args;
    va_start(args, a_format);
    vsnprintf(message_, sizeof(message_), a_format, args);
    va_end(args);
}

/**
 * @brief Exception constructor
 *
 * @param a_format  printf like format followed by a variable number of arguments
 */
osal::Exception::Exception (bool a_is_null, const char* a_format, ...) throw()
{
    is_null_ = a_is_null;
    line_    = 0;
    file_    = "";
    va_list args;
    va_start(args, a_format);
    vsnprintf(message_, sizeof(message_), a_format, args);
    va_end(args);
}

/**
 * @brief Exception copy construcor
 *
 * @param a_exception the exception to clone
 */
osal::Exception::Exception (const Exception& a_exception) throw()
{
    is_null_ = a_exception.is_null_;
    strcpy(message_, a_exception.Message());
    line_ = a_exception.Line();
    file_ = a_exception.File();
}

/**
 * @brief Destructor
 */
osal::Exception::~Exception () throw()
{
    /* empty */
}
