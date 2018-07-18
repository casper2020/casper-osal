/**
 * @file debug_trace.h
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

#ifndef OSAL_DEBUG_TRACE_H_
#define OSAL_DEBUG_TRACE_H_

    #if defined(_DEBUG) || defined(ENABLE_DEBUG) || defined(ENABLE_DEBUG_TRACE) /* make sure we're wanted */
        #include <stdio.h>

        #define DEBUGTRACEX(token,...)           osal::Debug::GetInstance()->LogCFL(token, __FUNCTION__, __LINE__, __VA_ARGS__);
        #define DEBUGTRACE(token,...)            osal::Debug::GetInstance()->LogC(token, __VA_ARGS__);
        #define DEBUGTRACE_SQLPROFILE(token,...) osal::Debug::GetInstance()->LogC(token, __VA_ARGS__);
        #define DEBUGIF(token) if (true)
        #define DEBUGTRACE_MUTEX(token, ...)

    #else   /* enable streamlining of the code */

        #define DEBUGTRACE_MUTEX(token, ...)
        #define DEBUGTRACEX(token, ...)
        #define DEBUGTRACE(token,...)
        #define DEBUGIF(x) if (false)
        #define DEBUGTRACE_SQLPROFILE(token,...)

    #endif

#endif // OSAL_DEBUG_TRACE_H_
