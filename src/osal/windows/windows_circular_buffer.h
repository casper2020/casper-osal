#pragma once
/**
 * @file WinCircularBuffer.h Lockless Circular buffer - header and inline functions
 *
 * Circular buffer without locks and continuos memory copy using two contiguous regions of virtual memory
 * this a POSIX/GCC port of the fantastic TPCircularBuffer made by Michael Tyson, original code is at
 * https://github.com/michaeltyson/TPCircularBuffer
 *
 * Adapted for WIN32 by Eurico Inocencio on 2012-10-28, MIT License kept
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
 * $Id: $
 */
#ifndef NRS_WINDOWS_CIRCULAR_BUFFER_H
#define NRS_WINDOWS_CIRCULAR_BUFFER_H
#include "osal/osalite.h"

#include <windows.h>
#include <stdint.h>
#include <stdlib.h>   // for NULL

namespace osal
{
    namespace windows
    {
        class CircularBuffer
        {
        protected: // data

            void*         buffer_;
            void*         upper_half_;
            void*         lower_half_;
            int32_t       length_;
            int32_t       tail_;
            int32_t       head_;
            HANDLE        map_handle_;
            volatile LONG fill_count_;

        public: // methods

            CircularBuffer ();
            virtual ~CircularBuffer ();

            bool  Init      (const char* a_data_path, int32_t a_length);
            void  Clear     ();
            void  Close     ();

        public: // methods

            // Reading (consuming)
            void* Tail         (int32_t* a_available_bytes);
            void  Consume      (int32_t  a_amount);

            // Writing (producing)
            bool  ProduceBytes (const void* a_source, int32_t a_length);
            void* Head         (int32_t* a_available_bytes);
            void  Produce      (int32_t  a_amount);

        protected:

            void  Init    ();

        };

    } // endof namespace streamer
} // endof namespace osal

inline osal::windows::CircularBuffer::CircularBuffer ()
{
    Init();
}

inline osal::windows::CircularBuffer::~CircularBuffer ()
{
    Close();
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
inline void* osal::windows::CircularBuffer::Tail (int32_t* a_available_bytes)
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
inline void osal::windows::CircularBuffer::Consume (int32_t a_amount)
{
    tail_ = (tail_ + a_amount) % length_;
    InterlockedExchangeAdd(&fill_count_, -a_amount);
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
inline void* osal::windows::CircularBuffer::Head (int32_t* a_available_bytes)
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
inline void osal::windows::CircularBuffer::Produce (int a_amount)
{
    head_ = (head_ + a_amount) % length_;
    InterlockedExchangeAdd(&fill_count_, a_amount);
}

#endif // NRS_WINDOWS_CIRCULAR_BUFFER_H
