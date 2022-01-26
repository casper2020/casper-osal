/**
 * @file about.h
 *
 * Copyright (c) 2011-2022 Cloudware S.A. All rights reserved.
 *
 * This file is part of casper-osal.
 *
 * casper-osal is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * casper-osal  is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with casper-osal. If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once
#ifndef CASPER_OSAL_ABOUT_H_
#define CASPER_OSAL_ABOUT_H_

namespace osal
{
    const char* const ABBR () __attribute__((used));
    const char* const NAME () __attribute__((used));
    const char* const VERSION () __attribute__((used));
    const char* const REL_NAME () __attribute__((used));
    const char* const REL_DATE () __attribute__((used));
    const char* const REL_BRANCH () __attribute__((used));
    const char* const REL_HASH () __attribute__((used));
    const char* const INFO () __attribute__((used));
}

#endif // CASPER_OSAL_ABOUT_H_
