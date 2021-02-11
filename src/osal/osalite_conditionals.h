#pragma once
/**
 * @file osal_conditionals.h Header of operating system abstraction layer
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

#ifndef NRS_OSAL_OSALITE_CONDITIONALS_H_
#define NRS_OSAL_OSALITE_CONDITIONALS_H_

#undef OSALITE_TARGET_WINDOWS_OS
#undef OSALITE_TARGET_WINDOWS_UNIVERSAL_OS
#undef OSALITE_TARGET_WINDOWS_UNIVERSAL_OS_PHONE
#undef OSALITE_TARGET_WINDOWS_UNIVERSAL_OS_DESKTOP

#undef OSALITE_TARGET_MAC_OS

#undef OSALITE_TARGET_IOS
#undef OSALITE_TARGET_IOS_DEVICE
#undef OSALITE_TARGET_IOS_SIMULATOR

#undef OSALITE_TARGET_ANDROID_OS

#if _WIN32

    #include <windows.h>

    #define OSALITE_TARGET_WINDOWS_OS _WIN32

    #if defined(WINAPI_FAMILY_PARTITION)

        #if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)

            #define OSALITE_TARGET_WINDOWS_UNIVERSAL_OS WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)

            #if WINAPI_FAMILY==WINAPI_FAMILY_PHONE_APP
                #define OSALITE_TARGET_WINDOWS_UNIVERSAL_OS_PHONE
            #else // WINAPI_FAMILY==WINAPI_FAMILY_PC_APP
                #define OSALITE_TARGET_WINDOWS_UNIVERSAL_OS_DESKTOP
            #endif

        #endif

    #endif

#elif defined(__APPLE__)

    #include "TargetConditionals.h"

    #if TARGET_OS_IPHONE
        #define OSALITE_TARGET_IOS_DEVICE TARGET_OS_IPHONE
        #define OSALITE_TARGET_IOS
    #elif defined(TARGET_OS_IPHONE_SIMULATOR)
        #define OSALITE_TARGET_IOS_SIMULATOR TARGET_OS_IPHONE_SIMULATOR
        #define OSALITE_TARGET_IOS
    #elif TARGET_OS_MAC
        #define OSALITE_TARGET_MAC_OS TARGET_OS_MAC
    #endif

#elif defined(ANDROID) || defined(__ANDROID__)

    #include <jni.h>

    #define OSALITE_TARGET_ANDROID_OS ANDROID

#endif

#endif // NRS_OSAL_OSALITE_CONDITIONALS_H_

