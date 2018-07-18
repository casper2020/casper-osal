#pragma once
/**
 * @file windows_condition_variable.h - condition variable windows, declaration
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

#ifndef NRS_OSAL_WINDOWS_WINDOWS_CONDITION_VARIABLE_H
#define NRS_OSAL_WINDOWS_WINDOWS_CONDITION_VARIABLE_H

#include "osal/mutex.h"
#include <windows.h>
#include <stdint.h>

namespace osal {

    namespace windows {

        class ConditionVariable {

        private: // data

            CRITICAL_SECTION    condition_lock_;
            CONDITION_VARIABLE  condition_;
            unsigned            count_;

        public: // constructor(s) / destructor

            ConditionVariable ();
            virtual ~ConditionVariable();

        public: // method(s) / function(s) declaration

            void Wait  ();
            void Wait  (int32_t a_timeout_in_secs);
            void Wake  ();

        };

    }
}

#endif // NRS_OSAL_WINDOWS_WINDOWS_CONDITION_VARIABLE_H
