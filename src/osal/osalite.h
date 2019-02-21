/**
 * @file osal.h Header of operating system abstraction layer
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
#pragma once
#ifndef NRS_OSAL_OSALITE_H_
#define NRS_OSAL_OSALITE_H_

#include "osal/osal_types.h"
#include "osal/osal_file.h"
#include "osal/osal_dir.h"
#include "osal/osal_random.h"
#include "osal/osalite_conditionals.h"
#include "osal/exception.h"
#include "osal/osal_singleton.h"
#include "osal/thread_helper.h"

#include <assert.h>

#include <inttypes.h>

#if !defined(CASPER_NO_ICU) && (defined(DEBUG) || defined(_DEBUG) || defined(ENABLE_DEBUG))

    #include "osal/debug/trace.h"

    #ifndef OSAL_DEBUG_STDOUT
        #define OSAL_DEBUG_STDOUT(...) fprintf(stdout, __VA_ARGS__)
    #endif

    #ifndef OSAL_DEBUG_STDERR
        #define OSAL_DEBUG_STDERR(...) fprintf(stderr, __VA_ARGS__)
    #endif

    #define OSALITE_DEBUG 1
    #define OSALITE_DEBUG_IF(a_token) if ( true == osal::debug::Trace::GetInstance().IsRegistered(a_token) )

    #ifndef OSAL_DEBUG_FUNC_C
        #define OSAL_DEBUG_FUNC_C(a_token) fprintf(stdout, "\n[C] %s\n",  __PRETTY_FUNCTION__);
    #endif

    #ifndef OSAL_DEBUG_FUNC_R
        #define OSAL_DEBUG_FUNC_R(a_token, a_format, ...) fprintf(stdout, "\n[R] %s\n" a_format, __PRETTY_FUNCTION__, __VA_ARGS__);
    #endif

    #ifndef OSAL_DEBUG_EXCEPTION
        #define OSAL_DEBUG_EXCEPTION(a_format, ...) osal::debug::Trace::GetInstance().LogExtended("exceptions",  __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__);
    #endif

    #ifndef OSAL_DEBUG_OSAL_EXCEPTION
        #define OSAL_DEBUG_OSAL_EXCEPTION(a_exception) osal::debug::Trace::GetInstance().LogExtended("exceptions",  __PRETTY_FUNCTION__, __LINE__, a_exception.Message());
    #endif

    #ifndef OSAL_DEBUG_STD_EXCEPTION
        #define OSAL_DEBUG_STD_EXCEPTION(a_exception) osal::debug::Trace::GetInstance().LogExtended("exceptions",  __PRETTY_FUNCTION__, __LINE__, a_exception.what());
    #endif

    #ifndef OSAL_DEBUG_STD_ERROR
        #define OSAL_DEBUG_STD_ERROR(a_error) osal::debug::Trace::GetInstance().LogExtended("errors",  __PRETTY_FUNCTION__, __LINE__,  a_error.what());
    #endif

    #ifndef OSALITE_REGISTER_DEBUG_TOKEN
        #define OSALITE_REGISTER_DEBUG_TOKEN(a_token, a_file) osal::debug::Trace::GetInstance().Register(a_token, a_file);
    #endif

    #ifndef OSALITE_DEBUG_TRACE
        #define OSALITE_DEBUG_TRACE(a_token, ...) osal::debug::Trace::GetInstance().LogExtended(a_token,  __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__);
    #endif

    #ifndef OSALITE_DEBUG_PRINT
        #define OSALITE_DEBUG_PRINT(a_token, ...) osal::debug::Trace::GetInstance().Log(a_token, __VA_ARGS__);
    #endif

    #ifndef OSALITE_ABORT
        #define OSALITE_ABORT() assert(1==2)
    #endif

    #ifndef OSALITE_ASSERT
        #define OSALITE_ASSERT(a_condition) assert(a_condition)
    #endif

    #undef OSALITE_DEBUG_SET_MAIN_THREAD_ID
    #define OSALITE_DEBUG_SET_MAIN_THREAD_ID() \
        osal::ThreadHelper::GetInstance().Start();

    #undef OSALITE_DEBUG_FAIL_IF_NOT_AT_MAIN_THREAD
    #define OSALITE_DEBUG_FAIL_IF_NOT_AT_MAIN_THREAD() \
        OSALITE_ASSERT(true == osal::ThreadHelper::GetInstance().AtMainThread())

    #undef OSALITE_DEBUG_FAIL_IF_NOT_AT_THREAD
    #define OSALITE_DEBUG_FAIL_IF_NOT_AT_THREAD(a_id)[&] { \
        return osal::ThreadHelper::GetInstance().CurrentThreadID() == a_id; \
    }

#else

    #undef OSAL_DEBUG_STDOUT
    #define OSAL_DEBUG_STDOUT(...)


    #undef OSAL_DEBUG_STDERR
    #define OSAL_DEBUG_STDERR(...)

    #undef OSALITE_DEBUG
    #define OSALITE_DEBUG_IF(a_token) if ( false )

    #undef OSAL_DEBUG_FUNC_C
    #define OSAL_DEBUG_FUNC_C(a_token)

    #undef OSAL_DEBUG_FUNC_R
    #define OSAL_DEBUG_FUNC_R(a_token, a_format, ...)

    #undef OSAL_DEBUG_EXCEPTION
    #define OSAL_DEBUG_EXCEPTION(a_format, ...)

    #undef OSAL_DEBUG_OSAL_EXCEPTION
    #define OSAL_DEBUG_OSAL_EXCEPTION(a_exception)

    #undef OSAL_DEBUG_STD_EXCEPTION
    #define OSAL_DEBUG_STD_EXCEPTION(a_exception)

    #undef OSAL_DEBUG_STD_ERROR
    #define OSAL_DEBUG_STD_ERROR(a_error)

    #undef OSALITE_REGISTER_DEBUG_TOKEN
    #define OSALITE_REGISTER_DEBUG_TOKEN(a_token, a_file)

    #undef OSALITE_DEBUG_TRACE
    #define OSALITE_DEBUG_TRACE(a_token, ...)

    #undef OSALITE_DEBUG_PRINT
    #define OSALITE_DEBUG_PRINT(a_token, ...)

    #undef OSALITE_ABORT
    #define OSALITE_ABORT()
 
    #undef OSALITE_ASSERT
    #define OSALITE_ASSERT(a_condition)

    #undef OSALITE_DEBUG_SET_MAIN_THREAD_ID
    #define OSALITE_DEBUG_SET_MAIN_THREAD_ID()

    #undef OSALITE_DEBUG_FAIL_IF_NOT_AT_MAIN_THREAD
    #define OSALITE_DEBUG_FAIL_IF_NOT_AT_MAIN_THREAD()

    #undef OSALITE_DEBUG_FAIL_IF_NOT_AT_THREAD
    #define OSALITE_DEBUG_FAIL_IF_NOT_AT_THREAD(a_id)

#endif

#if (defined(DEBUG) || defined(_DEBUG) || defined(ENABLE_DEBUG))

    #undef OSALITE_DECLARE_VAR_IF_DEBUG
    #define OSALITE_DECLARE_VAR_IF_DEBUG(a_type, a_name) a_type a_name

    #define OSALITE_DECLARE_SWATCH_IF_DEBUG(a_name) OSALITE_DECLARE_VAR_IF_DEBUG(osal::utils::Swatch, a_name)
    #define OSALITE_START_SWATCH_IF_DEBUG(a_swatch) a_swatch.Start()
    #define OSALITE_STOP_SWATCH_IF_DEBUG(a_swatch) a_swatch.Stop()
    #define OSALITE_TICKS_MS_SWATCH_IF_DEBUG(a_swatch) (a_swatch.Ticks() / 1000 )

#else

    #undef OSALITE_DECLARE_VAR_IF_DEBUG
    #define OSALITE_DECLARE_VAR_IF_DEBUG(a_type, a_name)

    #undef OSALITE_DECLARE_SWATCH_IF_DEBUG
    #define OSALITE_DECLARE_SWATCH_IF_DEBUG(a_name)
    #undef OSALITE_START_SWATCH_IF_DEBUG
    #define OSALITE_START_SWATCH_IF_DEBUG(a_swatch)
    #undef OSALITE_STOP_SWATCH_IF_DEBUG
    #define OSALITE_STOP_SWATCH_IF_DEBUG(a_swatch)
    #undef OSALITE_TICKS_MS_SWATCH_IF_DEBUG
    #define OSALITE_TICKS_MS_SWATCH_IF_DEBUG(a_swatch)

#endif

#ifdef __APPLE__
    #ifndef OSALITE_DO_PRAGMA
        #define OSALITE_DO_PRAGMA(x) _Pragma (#x)
        #define OSALITE_TODO(x) OSALITE_DO_PRAGMA(message ("WARNING: TODO - " #x))
    #endif
#else
    #undef OSALITE_TODO
    #define OSALITE_TODO(x)
#endif

/*
 * OSALITE NGX DEBUG LOG
 */
#undef OSALITE_NGX_LOG
#if defined(OSALITE_TARGET_MAC_OS)
    #define OSALITE_NGX_LOG(a_r, a_level, ...) { \
        OSAL_DEBUG_STDOUT(__VA_ARGS__); \
        ngx_log_error(a_level, a_r->connection->log, 0, __VA_ARGS__); \
    }
#else
    #define OSALITE_NGX_LOG(a_r, a_level, ...) ngx_log_error(a_level, a_r->connection->log, 0, __VA_ARGS__);
#endif

/*
 * OSALITE NORMALIZE PATH
 */
#define OSAL_NORMALIZE_PATH(a_path)[&]() { \
    std::string path = a_path; \
    if ( path.length() > 0 && path.c_str()[path.length()-1] != '/' ) { \
        path += '/'; \
    } \
    return path; \
}()

/*
 * C/ C++ COMPILER ATTRIBUTES
 */
#ifndef OSALITE_DISABLE_FUNCTION_OPTIMIZATION_ATTRIBUTE
    #ifdef __APPLE__
        #define OSALITE_DISABLE_FUNCTION_OPTIMIZATION_ATTRIBUTE __attribute__ ((optnone))
    #else
        #define OSALITE_DISABLE_FUNCTION_OPTIMIZATION_ATTRIBUTE __attribute__ ((optimize("-O0")))
    #endif
#endif

/**
 * @brief Outputs to console the string in <code>a_debug_str</code>
 * @param a_debug_str The string to display
 */
void OsalDebugOutput (OsalConstUtf8Ptr a_debug_str);

// Quick and DIRTY porting area
#ifdef _WIN32

// This include is pulling the correct windows headers TODO find them and remove this
#include "curl/mprintf.h"

#include <direct.h>
#include <time.h>

#define snprintf _snprintf
#define OsalSleepMs(MS) Sleep(MS)
#define strncasecmp _strnicmp
#define strcasecmp  _stricmp
#define mkdir       _mkdir
typedef size_t ssize_t;

#define OsalRandomSeed(V)    srand((unsigned int)(V))
#define OsalRandom()         rand()
#define OsalRandomInRange(V) ((rand() % (V))+1)

#else
// NOT windows
#include <unistd.h>
#include <strings.h>

#define OsalSleepMs(MS) usleep((MS)*1000)

#endif

// Local includes
#if defined(ANDROID) || defined(_WIN32)
    #define SIZET_FMT "%d"
    #define SIZET_FMT_BP(d)   "%" #d "d"
    #define SSIZET_FMT_BP(d)  "%" #d "d"
#else
    #define SIZET_FMT "%zd"
    #define SIZET_FMT_BP(d)   "%" #d "zu"
    #define SSIZET_FMT_BP(d)  "%" #d "zd"
#endif

//#ifdef __APPLE__

    #define INT8_FMT   "%" PRId8
    #define UINT8_FMT  "%" PRIu8
    #define UINT8_FMT_ZP(d) "%0" #d PRIu8

    #define INT16_FMT  "%" PRId16
    #define UINT16_FMT "%" PRIu16
    #define UINT16_FMT_ZP(d) "%0" #d PRIu16

    #define INT32_FMT  "%" PRId32
    #define UINT32_FMT "%" PRIu32

    #define INT64_FMT        "%" PRId64
    #define INT64_FMT_ZP(d)  "%0" #d PRId64

    #define INT64_FMT_MAX_RA "%19" PRId64

    #define UINT64_FMT       "%" PRIu64
    #define UINT64_FMT_ZP(d) "%0" #d PRIu64

    #define LONG_FMT  "%" PRIdPTR
    #define ULONG_FMT "%" PRIuPTR

    #define LONG_FMT  "%" PRIdPTR
    #define ULONG_FMT "%" PRIuPTR

    #define LONG_LONG_FMT   "%" "lld"
    #define U_LONG_LONG_FMT "%" "%llu"

    #define DOUBLE_FMT "%lf"

//#else
//
//    #if defined(__LP64__) || defined(_LP64)
//        #define INT64_FMT      "%ld"
//        #define UINT64_FMT     "%lu"
//    #else
//        #define INT64_FMT      "%lld"
//        #define UINT64_FMT     "%llu"
//    #endif
//#endif


// -9223372036854775807
#ifndef OSAL_INT64_STR_LEN
#define OSAL_INT64_STR_LEN sizeof("-9223372036854775808")
#endif

#ifndef OSAL_INT64_STR_LEN_WZ
#define OSAL_INT64_STR_LEN_WZ ( sizeof("-9223372036854775808") - 1)
#endif

// -2147483648
#ifndef OSAL_INT32_STR_LEN
#define OSAL_INT32_STR_LEN sizeof("-2147483648")
#endif

#ifndef OSAL_INT32_STR_LEN_WZ
#define OSAL_INT32_STR_LEN_WZ ( sizeof("-2147483648") - 1 )
#endif

// 65535
#ifndef OSAL_UINT16_STR_LEN
#define OSAL_UINT16_STR_LEN sizeof("65535")
#endif

#ifndef OSAL_UINT16_STR_LEN_WZ
#define OSAL_UINT16_STR_LEN_WZ ( sizeof("65535") - 1 )
#endif

// int
#define OSAL_INT_STR_LEN_WZ (sizeof(void*)== 8 ? OSAL_INT64_STR_LEN_WZ : OSAL_INT32_STR_LEN_WZ);

// ptr
#define OSAL_PTR_SIZE        (sizeof(void*))

#ifndef OSAL_MAX_UINT32_VALUE
    #if ((__GNU__ == 2) && (__GNUC_MINOR__ < 8))
        #define OSAL_MAX_UINT32_VALUE  (uint32_t) 0xffffffffLL
    #else
        #define OSAL_MAX_UINT32_VALUE  (uint32_t) 0xffffffff
    #endif
#endif // OSAL_MAX_UINT32_VALUE

#ifdef ANDROID
#include <jni.h>
extern JavaVM* g_java_vm; // Pointer to the process wide JavaVM
#define detach_thread_from_java()  if ( g_java_vm != NULL ) {  g_java_vm->DetachCurrentThread(); }
#else
#define detach_thread_from_java()
#endif

#ifdef __APPLE__
#define OSAL_HAVE_STATVFS
#define MAP_ANONYMOUS MAP_ANON

#endif

/**
 * @brief Outputs to console the string in <code>a_debug_str</code>
 * @param a_debug_str The string to display
 */

#define OsalDebugOutput(V)

#ifndef __APPLE__ // backtrace
    #include <stdio.h>
    #include <execinfo.h>
    #include <signal.h>
    #include <stdlib.h>
    #include <unistd.h>
#else
    #include <execinfo.h>
    #include <stdio.h>
#endif

#define OSALITE_BACKTRACE() \
[&] () { \
    void* callstack[128];\
    int i, frames = backtrace(callstack, 128); \
    char** strs = backtrace_symbols(callstack, frames); \
    fprintf(stderr, "[B] - OSALITE_BACKTRACE %s:%d \n", __PRETTY_FUNCTION__, (int)__LINE__); \
    for (i = 0; i < frames; ++i) { \
        fprintf(stderr, "%s\n", strs[i]); \
    } \
    fprintf(stderr, "[E] - OSALITE_BACKTRACE %s:%d \n", __PRETTY_FUNCTION__, (int)__LINE__); \
    free(strs);\
}()

#ifdef __APPLE__
    #define OSALITE_STD_CPP_GENERIC_EXCEPTION_TRACE() \
        [&] () -> std::string { \
            std::stringstream ss; \
            std::exception_ptr p = std::current_exception(); \
            ss << "C++ Generic Exception @" << __PRETTY_FUNCTION__ << ":" << __LINE__; \
            try { \
                std::rethrow_exception(p);\
            } catch(const std::exception& e) { \
                ss << "what() =" << e.what(); \
            } \
        return ss.str(); \
    }()
#else
    #define OSALITE_STD_CPP_GENERIC_EXCEPTION_TRACE() \
        [&] () -> std::string { \
            std::stringstream ss; \
            std::exception_ptr p = std::current_exception(); \
            ss << "C++ Generic Exception @" << __PRETTY_FUNCTION__ << ":" << __LINE__; \
            if ( p ) { \
                ss << "name() =" << p.__cxa_exception_type()->name(); \
                ss << "what() =" << p.__cxa_exception_type()->name(); \
            } \
        return ss.str(); \
    }()
#endif

#endif // NRS_OSAL_OSALITE_H_
