/**
 * @file thread_helper.h
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
#ifndef NRS_OSAL_THREAD_HELPER_H_
#define NRS_OSAL_THREAD_HELPER_H_

#include "osal/posix/posix_thread_helper.h"
namespace osal
{
    typedef osal::posix::ThreadHelper ThreadHelper;
}

#endif // NRS_OSAL_THREAD_HELPER_H_
