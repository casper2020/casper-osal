#pragma once
/**
 * @file CircularBufferNoMmap.h Lockless Circular buffer for systems w/o memory maps - header and inline functions
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
#ifndef NRS_OSAL_POSIX_CIRCULAR_BUFFER_NO_MMAP_H
#define NRS_OSAL_POSIX_CIRCULAR_BUFFER_NO_MMAP_H
#include "osal/osalite.h"
#include "osal/debug_trace.h"

#include <stdint.h>
#include <stdlib.h>   // for NULL
#include <sys/mman.h> // for mmap

namespace osal
{
    namespace posix
    {
        class CircularBufferNoMmap
        {
        protected: // data

            void*             buffer_;
            int32_t           length_;
            int32_t           tail_;
            int32_t           contiguous_readable_bytes_;
            int32_t           head_;
            int32_t           contiguous_writable_bytes_;
            bool              seekable_;
            volatile int32_t  fill_count_;

        public: // methods

            CircularBufferNoMmap ();
            virtual ~CircularBufferNoMmap ();

            bool  Init      (const char* a_data_path, int32_t a_length);
            void  Clear     ();
            void  Close     ();

        public: // methods

            // Reading (consuming)
            int32_t ReadableBytes  ();
            void    Read           (void* a_dst, int32_t a_amount);

            // Writing (producing)
            int32_t WritableBytes  ();
            void    Write          (void* a_src, int32_t a_amount);

            // Seek mode for random access (*not* thread safe)
            int32_t  Size              () const;
            int      Seek              (int64_t a_offset, int a_whence);
            long int Tell              () const;
            bool     IsSeekable        () const;
            int      SeekableReadSize  () const;
            bool     IsOperational     () const;
            bool     HeadWrapsArround  (int32_t a_requested_bytes) const;
            void     Trace             (const char* a_tag, int32_t a_amount);

        protected:

            void  Init    ();

        };

    } // endof namespace streamer
} // endof namespace osal

inline osal::posix::CircularBufferNoMmap::CircularBufferNoMmap ()
{
    Init();
}

inline osal::posix::CircularBufferNoMmap::~CircularBufferNoMmap ()
{
    Close();
}

inline int32_t osal::posix::CircularBufferNoMmap::Size () const
{
    return length_;
}

/**
 * Access end of buffer
 *
 *  This gives you a pointer to the end of the buffer, ready
 *  for reading, and the number of available bytes to read.
 *
 * @param o_available_bytes On output, the number of bytes ready for reading
 * @return Pointer to the first bytes ready for reading, or NULL if buffer is empty
 */
inline int32_t osal::posix::CircularBufferNoMmap::ReadableBytes ()
{
    int32_t fc = fill_count_;

    if ( fc == 0 ) {
        contiguous_readable_bytes_ = 0;
        DEBUGIF("CBF") {
            Trace("Tail", 0);
        }
        return 0;
    }
    if ( tail_ + fc >= length_ ) {
        // H < T
        contiguous_readable_bytes_ = length_ - tail_;
    } else {
        // H > T
        contiguous_readable_bytes_ = fc;
    }
    DEBUGIF("CBF") {
        Trace("Tail", fc);
    }
    return fc;
}

/**
 * Consume bytes in buffer
 *
 *  This frees up the just-read bytes, ready for writing again.
 *
 * @param a_amount Number of bytes to consume
 */
inline void osal::posix::CircularBufferNoMmap::Read (void* a_dst, int32_t a_amount)
{
    if ( contiguous_readable_bytes_ == 0 ) {
        return;
    }
    if ( a_amount <=  contiguous_readable_bytes_ ) {
        memcpy(a_dst, (uint8_t*) buffer_ + tail_, a_amount);
        tail_ += a_amount;
    } else {
        memcpy(a_dst, (uint8_t*) buffer_ + tail_, contiguous_readable_bytes_);
        memcpy((uint8_t*) a_dst + contiguous_readable_bytes_, buffer_, a_amount - contiguous_readable_bytes_);
        tail_     = a_amount - contiguous_readable_bytes_;
        seekable_ = false;
    }
    __sync_add_and_fetch(&fill_count_, -a_amount);
}

/**
 * Access front of buffer
 *
 *  This gives you a pointer to the front of the buffer, ready
 *  for writing, and the number of available bytes to write.
 *
 * @param o_available_bytes On output, the number of contiguous bytes ready for writing
 * @param o_total_free_bytes On output, the total free space in the buffer
 * @return Pointer to the first bytes ready for writing, or NULL if buffer is full
 */
inline int32_t osal::posix::CircularBufferNoMmap::WritableBytes ()
{
    int32_t fc = fill_count_;

    if ( fc == length_ ) {
        contiguous_writable_bytes_ = 0;
        DEBUGIF("CBF") {
            Trace("Head", 0);
        }
        return 0;
    }
    if ( head_ >= fc ) {
        // H > T
        contiguous_writable_bytes_ = length_ - head_;
    } else {
        // H < T
        contiguous_writable_bytes_ = length_ - fc;
    }
    DEBUGIF("CBF") {
        Trace("Head", length_ - fc);
    }
    return length_ - fc;
}

inline void osal::posix::CircularBufferNoMmap::Write (void* a_src, int32_t a_amount)
{
    if ( contiguous_writable_bytes_ == 0 ) {
        return;
    }
    if ( a_amount <=  contiguous_writable_bytes_ ) {
        memcpy((uint8_t*) buffer_ + head_, a_src, a_amount);
        head_ += a_amount;
    } else {
        memcpy((uint8_t*) buffer_ + head_, a_src, contiguous_writable_bytes_);
        memcpy(buffer_, (uint8_t*) a_src + contiguous_writable_bytes_, a_amount - contiguous_writable_bytes_);
        head_     = a_amount - contiguous_writable_bytes_;
        seekable_ = false;
    }
    __sync_add_and_fetch(&fill_count_, a_amount);
}


/**
 * @brief Seek inside the buffer, buffer must be seekable and this is *NOT* thread safe
 *
 * @param a_offset Offset to seek
 * @param a_whence Seek mode
 *     @li SEEK_SET to make the offset relative to the beginning of the resource
 *     @li SEEK_CUR to make the offset relative to the current position
 *     @li SEEK_END to make the offset relative to the end position (and count backwards)
 * @return 0 for success, -1 for ERROR: Bad Stream, -2 for ERROR: End of Stream
 */
inline int osal::posix::CircularBufferNoMmap::Seek (int64_t a_offset, int a_whence)
{
    int     rv = -1;

    // Americo! can't be called before all bytes are in buffer
    if ( true == seekable_ ) {
        int32_t offset = -1;

        switch(a_whence) {
            case SEEK_SET:
                offset = (int32_t) a_offset;
                break;
            case SEEK_CUR:
                offset = tail_ + (int32_t) a_offset;
                break;
            case SEEK_END:
                offset = head_ - (int32_t) a_offset;
                break;
        }
        if ( offset >= 0 && offset <= (int32_t) head_ ) { // note <= is needed to place SEEK_END past last byte
            tail_ = offset;
            fill_count_ = head_ - offset;
            rv = 0;
        }
    }
    return rv;
}

/**
 * @brief Return the current read offset of seekable stream in memory
 * @return The current offset in the tream or -1 on error
 */
inline long int osal::posix::CircularBufferNoMmap::Tell () const
{
    if ( true == seekable_ ) {
        return tail_;
    } else {
        return -1;
    }
}

inline int osal::posix::CircularBufferNoMmap::SeekableReadSize () const
{
    if ( true == seekable_ ) {
        return head_;
    } else {
        return -1;
    }
}

inline bool osal::posix::CircularBufferNoMmap::IsOperational () const
{
    if ( buffer_ != NULL && length_ > 0 ) {
        return true;
    } else {
        return false;
    }
}

inline bool osal::posix::CircularBufferNoMmap::HeadWrapsArround (int32_t a_requested_bytes) const
{
    if ( head_ + a_requested_bytes > length_) {
        return true;
    } else {
        return false;
    }
}

#endif // NRS_OSAL_POSIX_CIRCULAR_BUFFER_NO_MMAP_H

