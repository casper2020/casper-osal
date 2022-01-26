/**
 * @file about.cc
 *
 * Copyright (c) 2011-2022 Cloudware S.A. All rights reserved.
 *
 * This file is part of casper-connectors.
 *
 * casper-connectors is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * casper-connectors  is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with casper-connectors. If not, see <http://www.gnu.org/licenses/>.
 */

#include "osal/about.h"
#include "osal/version.h"

const char* const osal::ABBR ()
{
    return CASPER_OSAL_ABBR;
}

const char* const osal::NAME ()
{
    return CASPER_OSAL_NAME;
}

const char* const osal::VERSION ()
{
    return CASPER_OSAL_VERSION;
}

const char* const osal::REL_NAME ()
{
    return CASPER_OSAL_REL_NAME;
}

const char* const osal::REL_DATE ()
{
    return CASPER_OSAL_REL_DATE;
}

const char* const osal::REL_BRANCH ()
{
    return CASPER_OSAL_REL_BRANCH;
}

const char* const osal::REL_HASH ()
{
    return CASPER_OSAL_REL_HASH;
}

const char* const osal::INFO ()
{
    return CASPER_OSAL_INFO;
}
