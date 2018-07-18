#pragma once
/**
 * @file condition_variable - Header mux that pulls the condition variable implementation for each platform
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
#ifndef NRS_OSAL_CONDITION_VARIABLE_H_
#define NRS_OSAL_CONDITION_VARIABLE_H_

#if 0 // def _WIN32 MARTELADA_TO_REMOVE_WIN32
    #include "osal/windows/windows_condition_variable.h"
    namespace osal
    {
		typedef osal::windows::ConditionVariable ConditionVariable;
    }
#else
    #include "osal/posix/posix_condition_variable.h"
    namespace osal
    {
        typedef osal::posix::ConditionVariable ConditionVariable;
    }
#endif

#endif // NRS_OSAL_CONDITION_VARIABLE_H_
