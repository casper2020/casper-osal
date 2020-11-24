#pragma once
/**
 * @file singleton.h - NOT Thread safe singleton
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

#ifndef NRS_OSAL_SINGLETON_H_
#define NRS_OSAL_SINGLETON_H_
#pragma once

#include <type_traits>

#include <inttypes.h> // PRIu64

#if !defined(__APPLE__)
    #include <unistd.h>
    #include <sys/syscall.h>
#else
    #include <pthread.h>
#endif

#if 0 && ( defined(NDEBUG) && !( defined(DEBUG) || defined(_DEBUG) || defined(ENABLE_DEBUG) ) )
    #include <stdio.h>  // vsnprintf
    #include <stdarg.h> // va_list, va_start, va_arg, va_end
    #include <typeinfo>

    #define OSAL_SINGLETON_DEBUG_TRACE(a_format, ...) \
        do { \
            fprintf(stdout, a_format, __VA_ARGS__); \
            fflush(stdout); \
        } while (0)
#else
    #define OSAL_SINGLETON_DEBUG_TRACE(a_format, ...)
#endif

#if !defined(__APPLE__)
    #include <unistd.h>
    #include <sys/syscall.h>
#else
    #include <pthread.h>
#endif
#ifdef __APPLE__
    #define OSAL_SINGLETON_THEAD_ID()[]() { \
        uint64_t thread_id; \
        (void)pthread_threadid_np(NULL, &thread_id); \
        return thread_id; \
    }()
#else
    #define OSAL_SINGLETON_THEAD_ID()[]() { \
        return (uint64_t)syscall(SYS_gettid); \
    }()
#endif

#include <cxxabi.h>
#include <memory> // std::unique_ptr
#include <string>
#define OSAL_SINGLETON_TYPE_DEMANGLE(a_name)[]() -> std::string { \
    int status = -4; \
    std::unique_ptr<char, void(*)(void*)> res { \
        abi::__cxa_demangle(a_name, NULL, NULL, &status), \
        std::free \
    }; \
    return std::string( (status==0) ? res.get() : a_name ); \
} ()

namespace osal
{

    // -- -- //
    template <typename C> class Initializer
    {
        
    protected: // Refs
        
        C& instance_;
        
    public: // Constructor(s) / Destructor
        
        Initializer (C& a_instance);
        virtual ~Initializer ();
        
    };

    template <typename C> Initializer<C>::Initializer (C& a_instance)
        : instance_(a_instance)
    {
        OSAL_SINGLETON_DEBUG_TRACE("\t⌁ [%p @ %8" PRIu64 " ✔︎] : %s\n", this, OSAL_SINGLETON_THEAD_ID(), OSAL_SINGLETON_TYPE_DEMANGLE(typeid(this).name()).c_str());
    }

    template <typename C> Initializer<C>::~Initializer ()
    {
        OSAL_SINGLETON_DEBUG_TRACE("\t⌁ [%p @ %8" PRIu64 " ✗] : %s\n", this, OSAL_SINGLETON_THEAD_ID(), OSAL_SINGLETON_TYPE_DEMANGLE(typeid(this).name()).c_str());
    }

    // -- -- //
    template <typename T, typename I, typename = std::enable_if<std::is_base_of<Initializer<T>, I>::value>> class Singleton
    {

    public: // Constructor(s))

        Singleton (const Singleton<T,I>&) = delete; // Copy constructor
        Singleton (Singleton<T,I>&&)      = delete; // Move constructor

    protected: // Constructor // Destructor

        Singleton ()
        {
            OSAL_SINGLETON_DEBUG_TRACE("⌁ [%p @ %8" PRIu64  " ✔︎] : %s\n", this, OSAL_SINGLETON_THEAD_ID(), OSAL_SINGLETON_TYPE_DEMANGLE(typeid(T).name()).c_str());
        }

        ~Singleton()
        {
            OSAL_SINGLETON_DEBUG_TRACE("⌁ [%p @ %8" PRIu64  " ✗] : %s\n", this, OSAL_SINGLETON_THEAD_ID(), OSAL_SINGLETON_TYPE_DEMANGLE(typeid(T).name()).c_str());
        }
        
    public: // Operator(s) Overload

        Singleton<T,I>& operator=(Singleton<T,I> &&)                   = delete; // Move assign
        Singleton<T,I>& operator = (const Singleton<T,I>& a_singleton) = delete; // Copy assign

    public: // Inline Method(s) / Function(s)

        static T& GetInstance () __attribute__ ((visibility ("default")))
        {
            // C++11 'magic statics' aproatch
            static T t;
            static I i(t);
            return t;
        }       

    }; // end of template Singleton

} // end of namespace osal

#endif // NRS_OSAL_SINGLETON_H_
