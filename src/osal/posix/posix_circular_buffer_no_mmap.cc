/**
 * @file CircularBufferNoMmap.h Lockless Circular buffer for systems w/o memory maps - implementation
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

#include "osal/posix/posix_circular_buffer_no_mmap.h"
#include "osal/debug_trace.h"
#include <string.h>   // for memcpy
#include <unistd.h>   // for close, ftruncate
#include <fcntl.h>    // for open
#include <stdio.h>    // Temporary

#ifdef __ANDROID__
#undef perror
#define perror(V) DEBUGTRACE("CircularBufferNoMmap", V)
#endif

void osal::posix::CircularBufferNoMmap::Init ()
{
    buffer_     = NULL;
    length_     = 0;
    tail_       = 0;
    head_       = 0;
    fill_count_ = 0;
    seekable_   = true;
}

void osal::posix::CircularBufferNoMmap::Trace (const char* a_tag, int32_t a_amount)
{
    OSAL_UNUSED_PARAM(a_tag);
    OSAL_UNUSED_PARAM(a_amount);
    DEBUGTRACE("CBF", "%10.10s H=%10.10d T=%10.10d FC=%8.8d S=%c A=%d", a_tag, head_, tail_, fill_count_, seekable_ ? 'T' : 'F', a_amount);
}

/**
 * @brief Initialise buffer
 *
 * @param a_length Length of buffer
 */
bool osal::posix::CircularBufferNoMmap::Init (const char*, int32_t a_length)
{
    buffer_ = malloc(static_cast<size_t>(a_length));
    if ( buffer_ != NULL ) {
        length_ = a_length;
        return  true;
    } else {
        Close();
        return false;
    }
}

/**
 * @brief Cleanup buffer
 *
 * Releases buffer resources.
 */
void osal::posix::CircularBufferNoMmap::Close ()
{
    if ( buffer_ != NULL ) {
        free(buffer_);
    }
    Init();
}

/**
 * @brief Clear buffer
 *
 * Resets buffer to original, empty state.
 *
 * This is safe for use by consumer while producer is accessing
 * buffer.
 */
void osal::posix::CircularBufferNoMmap::Clear ()
{
    __sync_add_and_fetch(&fill_count_, -fill_count_);
    head_ = 0;
    tail_ = 0;
    seekable_ = true;
    DEBUGIF("CBF") {
        Trace("Clear", 0);
    }
}
