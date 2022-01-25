/**
 * @file version.h
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
#ifndef CASPER_OSAL_VERSION_H_
#define CASPER_OSAL_VERSION_H_

#ifndef CASPER_OSAL_ABBR
#define CASPER_OSAL_ABBR "co"
#endif

#ifndef CASPER_OSAL_NAME
#define CASPER_OSAL_NAME "casper-osal@b.n.s@"
#endif

#ifndef CASPER_OSAL_VERSION
#define CASPER_OSAL_VERSION "x.x.x"
#endif

#ifndef CASPER_OSAL_REL_DATE
#define CASPER_OSAL_REL_DATE "r.r.d"
#endif

#ifndef CASPER_OSAL_REL_BRANCH
#define CASPER_OSAL_REL_BRANCH "r.r.b"
#endif

#ifndef CASPER_OSAL_REL_HASH
#define CASPER_OSAL_REL_HASH "r.r.h"
#endif

#ifndef CASPER_OSAL_INFO
#define CASPER_OSAL_INFO CASPER_OSAL_NAME " v" CASPER_OSAL_VERSION
#endif

#define CASPER_OSAL_BANNER \
"  ____    _    ____  ____  _____ ____        ___  ____    _    _     " \
" / ___|  / \\  / ___||  _ \\| ____|  _ \\      / _ \\/ ___|  / \\  | |    " \
"| |     / _ \\ \\___ \\| |_) |  _| | |_) |____| | | \\___ \\ / _ \\ | |    " \
"| |___ / ___ \\ ___) |  __/| |___|  _ <_____| |_| |___) / ___ \\| |___ " \
" \\____/_/   \\_\\____/|_|   |_____|_| \\_\\     \\___/|____/_/   \\_\\_____|"
#endif // CASPER_OSAL_VERSION_H_
