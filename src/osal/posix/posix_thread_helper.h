/**
 * @file posix_thread_helper.h
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
#ifndef NRS_OSAL_POSIX_THREAD_HELPER_H_
#define NRS_OSAL_POSIX_THREAD_HELPER_H_

#include <pthread.h>
#include "osal/osal_singleton.h"
#include "osal/exception.h"

#if !defined(__APPLE__)
    #include <unistd.h>
    #include <sys/syscall.h> 
#endif

namespace osal
{
    
    namespace posix
    {
        
        class ThreadHelper : public osal::Singleton<ThreadHelper>
        {
            
        public: // Types
            
            typedef uint64_t ThreadID;
            
        public: // Static Const
            
            static const ThreadID k_invalid_thread_id_;
            
        private: // Static Data
            
            static ThreadID main_thread_id_;
            
        public: // Inline Method(s) / Function(s)
            
            void     Start ();
            bool     AtMainThread    () const;
            ThreadID CurrentThreadID () const;
            void     SetName (const std::string& a_name) const;
            
        };
        
        /**
         * @brief Mark the current thread as the 'main' one.
         */
        inline void ThreadHelper::Start ()
        {
            main_thread_id_ = CurrentThreadID();
        }
        
        /**
         * @brief Check if this function is being called at 'main' thread.
         */
        inline bool ThreadHelper::AtMainThread () const
        {
            return 0 != main_thread_id_ && CurrentThreadID() == main_thread_id_;
        }
        
        /**
         * @return The current thread id.
         *
         * @throw An exception when it's not possible to retrieve the current thread id.
         */
        inline ThreadHelper::ThreadID ThreadHelper::CurrentThreadID () const
        {
			#ifdef __APPLE__
                uint64_t thread_id;
                int rv = pthread_threadid_np(NULL, &thread_id);
                if ( 0 != rv ) {
                    throw OSAL_EXCEPTION_NA("Unable to fetch current thread id!");
                }
                return thread_id;
			#else
			    return (uint64_t)syscall(SYS_gettid);
			#endif
        }
    
        /**
         * @brief Set thread name.
         * *
         * @param a_name
         */
        inline void ThreadHelper::SetName (const std::string& a_name) const
        {
             #ifdef __APPLE__
                 pthread_setname_np(a_name.c_str());
             #else
                 pthread_setname_np(pthread_self(), a_name.c_str());
             #endif
        }
        
    } // end of namespace posix
    
} // end of namespace osal

#endif // NRS_OSAL_POSIX_THREAD_HELPER_H_
