#pragma once
/**
 * @file types.h Some types and portability macros
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

#ifndef NRS_OSAL_OSAL_TYPES_H_
#define NRS_OSAL_OSAL_TYPES_H_

#include <stdint.h>
#include <inttypes.h>

#ifndef NULL
#define NULL 0	//!< NULL value
#endif

#ifndef OSAL_UNUSED_PARAM
#define OSAL_UNUSED_PARAM(x) (void)x			//!< This macro can be used to remove the warning about unused function/method parameters
#endif

#if !defined(MIN)
#define MIN(A,B)	((A) < (B) ? (A) : (B))		//!< This macro can be used to retrieve the minimum of two expressions
#endif

#if !defined(MAX)
#define MAX(A,B)	((A) > (B) ? (A) : (B))		//!< This macro can be used to retrieve the maximum of two expressions
#endif

#define C3STR(x) const_cast<const OsalUtf8Ptr>(x)

extern "C" {

	typedef char				OsalUtf8Byte;			//!< typedef to char
	typedef OsalUtf8Byte*       OsalUtf8Ptr;			//!< typedef to OsalUtf8Byte*
    typedef const OsalUtf8Byte* OsalConstUtf8Ptr;		//!< typedef to const OsalUtf8Byte*

    typedef uint8_t             OsalByte;				//!< typedef to uint8_t
	typedef uint8_t*            OsalByteBufferPtr;		//!< typedef to uint8_t*
    typedef const uint8_t*      OsalConstByteBufferPtr; //!< typedef to const uint8_t*

	/**
	 * @enum OsalBoolean
	 * Defines the values for true and false
	 */
	typedef enum {
		kOsalBooleanFalse,	//!< False
		kOsalBooleanTrue	//!< True
	} OsalBoolean;

	/**
	 * @brief Checks if a float value is not a number
	 *
	 * @return Returns <code>OsalBooleanTrue</code> if <code>aFloatValue</code>
	 * is Nan, <code>OsalBooleanFalse</code> otherwise
	 */
	OsalBoolean OsalIsFloatNan (float aFloatValue);

	/**
	 * @brief Checks if a double value is not a number
	 *
	 * @return Returns <code>OsalBooleanTrue</code> if <code>aDoubleValue</code>
	 * is Nan, <code>OsalBooleanFalse</code> otherwise
	 */
	OsalBoolean OsalIsDoubleNan (double aDoubleValue);

    /**
     * @brief Returns platform float epsilon
     */
    float OsalFloatEpsilon ();
}

#endif // NRS_OSAL_OSAL_TYPES_H_

