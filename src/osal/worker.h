#pragma once
/**
 * @file worker.h - Header mux that pulls the Wor for each platform
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
#ifndef NRS_OSAL_WORKER_H
#define NRS_OSAL_WORKER_H

#if defined(__APPLE__) || defined(__ANDROID__) || 1 // MARTELADA_TO_REMOVE_WIN32
    #include "osal/posix/posix_worker.h"
    namespace osal
    {
        typedef osal::posix::Worker Worker;
    }
#elif _WIN32
    #include "osal/windows/windows_worker.h"
    namespace osal
    {
        typedef osal::windows::Worker Worker;
    }
#else
    #error "Sorry PAL there is no Worler support for this platform yet"
#endif

#endif // NRS_OSAL_WORKER_LOOP_H
