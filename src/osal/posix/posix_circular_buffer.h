#pragma once
/**
 * @file CircularBuffer.h Lockless Circular buffer - header and inline functions
 *
 * Circular buffer without locks and continuos memory copy using two contiguous regions of virtual memory
 * this a POSIX/GCC port of the fantastic TPCircularBuffer made by Michael Tyson, original code is at
 * https://github.com/michaeltyson/TPCircularBuffer
 *
 * Adapted for POSIX / GCC by Eurico Inocencio on 2012-10-28, MIT License kept
 *
 * Circular/Ring buffer implementation
 *
 * Created by Michael Tyson on 10/12/2011.
 * Copyright 2011-2012 A Tasty Pixel. All rights reserved.
 *
 * This implementation makes use of a virtual memory mapping technique that inserts a virtual copy
 * of the buffer memory directly after the buffer's end, negating the need for any buffer wrap-around
 * logic. Clients can simply use the returned memory address as if it were contiguous space.
 *
 * The implementation is thread-safe in the case of a single producer and single consumer.
 *
 * Virtual memory technique originally proposed by Philip Howard (http://vrb.slashusr.org/), and
 * adapted to Darwin by Kurt Revis (http://www.snoize.com,
 * http://www.snoize.com/Code/PlayBufferedSoundFile.tar.gz)
 *
 */
#ifndef NRS_OSAL_POSIX_POSIX_CIRCULARBUFFER_H
#define NRS_OSAL_POSIX_POSIX_CIRCULARBUFFER_H
#include "osal/osalite.h"

#include <stdint.h>
#include <stdlib.h>   // for NULL
#include <sys/mman.h> // for mmap

namespace osal
{
    namespace posix
    {
        class CircularBuffer
        {
        protected: // data

            void*             buffer_;
            void*             upper_half_;
            void*             lower_half_;
            int32_t           length_;
            int32_t           tail_;
            int32_t           head_;
            bool              seekable_;
            volatile int32_t  fill_count_;

        public: // methods

            CircularBuffer ();
            virtual ~CircularBuffer();

            bool  Init      (const char* a_data_path, int32_t a_length);
            void  Clear     ();
            void  Close     ();

        public: // methods

            // Reading (consuming)
            void*   Tail         (int32_t* a_available_bytes);
            void    Consume      (int32_t  a_amount);

            // Writing (producing)
            bool    ProduceBytes (const void* a_source, int32_t a_length);
            void*   Head         (int32_t* a_available_bytes);
            void    Produce      (int32_t  a_amount);

            // Seek mode for random access (*not* thread safe)
            int32_t  Size             () const;
            int      Seek             (int64_t a_offset, int a_whence);
            long int Tell             () const;
            bool     IsSeekable       () const;
            int      SeekableReadSize () const;
            void     ClearAndRewind   ();
            bool     IsOperational    ();


        protected:

            void  Init    ();

        };

    } // endof namespace streamer
} // endof namespace osal

inline osal::posix::CircularBuffer::CircularBuffer ()
{
    Init();
}

inline osal::posix::CircularBuffer::~CircularBuffer ()
{
    Close();
}

inline int32_t osal::posix::CircularBuffer::Size () const
{
    return length_;
}

/**
 * Access end of buffer
 *
 *  This gives you a pointer to the end of the buffer, ready
 *  for reading, and the number of available bytes to read.
 *
 * @param a_available_bytes On output, the number of bytes ready for reading
 * @return Pointer to the first bytes ready for reading, or NULL if buffer is empty
 */
inline void* osal::posix::CircularBuffer::Tail (int32_t* a_available_bytes)
{
    *a_available_bytes = fill_count_;
    if ( *a_available_bytes == 0 ) {
        return NULL;
    }
    return (void*)((char*) buffer_ + tail_);
}

/**
 * Consume bytes in buffer
 *
 *  This frees up the just-read bytes, ready for writing again.
 *
 * @param a_amount Number of bytes to consume
 */
inline void osal::posix::CircularBuffer::Consume (int32_t a_amount)
{
    tail_ = (tail_ + a_amount) % length_;
    __sync_add_and_fetch(&fill_count_, -a_amount);
}

/**
 * Access front of buffer
 *
 *  This gives you a pointer to the front of the buffer, ready
 *  for writing, and the number of available bytes to write.
 *
 * @param a_available_bytes On output, the number of bytes ready for writing
 * @return Pointer to the first bytes ready for writing, or NULL if buffer is full
 */
inline void* osal::posix::CircularBuffer::Head (int32_t* a_available_bytes)
{
    *a_available_bytes = (length_ - fill_count_);
    if ( *a_available_bytes == 0 ) {
        return NULL;
    }
    return (void*)((char*) buffer_ + head_);
}

/**
 * Produce bytes in buffer
 *
 *  This marks the given section of the buffer ready for reading.
 *
 * @param a_amount Number of bytes to produce
 */
inline void osal::posix::CircularBuffer::Produce (int a_amount)
{
    if ( head_ + a_amount >= length_ ) {
        head_     = (head_ + a_amount) % length_;
        seekable_ = false;
    } else {
        head_ += a_amount;
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
inline int osal::posix::CircularBuffer::Seek (int64_t a_offset, int a_whence)
{
    int     rv = -1;

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
inline long int osal::posix::CircularBuffer::Tell () const
{
    if ( true == seekable_ ) {
        return tail_;
    } else {
        return -1;
    }
}

inline int osal::posix::CircularBuffer::SeekableReadSize () const
{
    if ( true == seekable_ ) {
        return head_;
    } else {
        return -1;
    }
}

inline void osal::posix::CircularBuffer::ClearAndRewind ()
{
    tail_       = 0;
    head_       = 0;
    fill_count_ = 0;
    seekable_   = true;
}

inline bool osal::posix::CircularBuffer::IsOperational ()
{
    if ( buffer_ != MAP_FAILED && length_ > 0 ) {
        return true;
    } else {
        return false;
    }
}

#endif // NRS_OSAL_POSIX_POSIX_CIRCULARBUFFER_H
