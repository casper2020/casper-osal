#pragma once
/**
 * @file windows_worker.h - Simple thread based worker for windows, declaration
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

#ifndef NRS_OSAL_WINDOWS_WINDOWS_WORKER_H
#define NRS_OSAL_WINDOWS_WINDOWS_WORKER_H

#include <stdio.h>
#include <windows.h>
#include "osal/mutex.h"
#include "osal/condition_variable.h"

const DWORD MS_VC_EXCEPTION=0x406D1388;

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
   DWORD dwType; // Must be 0x1000.
   LPCSTR szName; // Pointer to name (in user addr space).
   DWORD dwThreadID; // Thread ID (-1=caller thread).
   DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

namespace osal
{
	namespace windows
	{
		class Worker
		{
		protected:

			HANDLE				thread_handle_;			//!< Posix Thread handle

			CRITICAL_SECTION	run_condition_lock_;	//!< Mutex to project access to state variable
            CONDITION_VARIABLE  run_condition_;			//!< Condition var used to signal changes in thread state
			bool				running_;				//!< True while the worker is running
			bool				run_worker_;
			int					startup_err_;		//!< It will be non-zero if the constructor fails
			const char*			name_;

		public: // methods

			Worker (const char* a_name, bool a_high_priority = false);
			virtual ~Worker ();

			virtual int   WorkerRunLoop        ();
			void          StartWorkerThread    ();
			void          Abort                ();
			virtual void  WorkerFunction       () = 0;
			static  void* WorkerRunLoopWrapper (void* a_self);
			static DWORD __stdcall WorkerRunLoopWrapperInternal(LPVOID);
		};

    } // namespace posix
} // namespace osal

#endif // #ifndef NRS_OSAL_WINDOWS_WINDOWS_WORKER_H
