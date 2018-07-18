#pragma once
/**
 * @file posix_condition_variable.cc - posix condition variable declaration
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

#ifndef NRS_OSAL_POSIX_POSIX_CONDITION_VARIABLE_H
#define NRS_OSAL_POSIX_POSIX_CONDITION_VARIABLE_H

#include <pthread.h>
#include <sys/types.h>
#include <stdint.h>

namespace osal {

    namespace posix {

        class ConditionVariable {

        private: // data

            pthread_mutex_t condition_lock_;
            pthread_cond_t  condition_;
            unsigned        count_;

        public: // constructor(s) / destructor

                    ConditionVariable ();
            virtual ~ConditionVariable();

        public: // method(s) / function(s) declaration

            void Wait  ();
            void Wait  (const int64_t a_timeout_seconds);
            void Wake  ();

        };

    }
}

#endif // NRS_OSAL_POSIX_POSIX_CONDITION_VARIABLE_H
