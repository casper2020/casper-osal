/**
 * @file CircularBuffer.h Lockless Circular buffer - implementation
 *
 * Circular buffer without locks and continuos memory copy using two contiguous regions of virtual memory
 * this is a Windoze port of the fantastic TPCircularBuffer made by Michael Tyson, original code is at
 * https://github.com/michaeltyson/TPCircularBuffer
 *
 * Adapted for WIN32 by Eurico Inocencio 2013-04-01, MIT License kept
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

#include "osal/windows/windows_circular_buffer.h"
#include "osal/debug_trace.h"
#include <string.h>   // for memcpy
#include <fcntl.h>    // for open

void osal::windows::CircularBuffer::Init ()
{
    lower_half_  = NULL;
    upper_half_  = NULL;
    buffer_      = NULL;
    map_handle_  = INVALID_HANDLE_VALUE;
    length_      = 0;
    tail_        = 0;
    head_        = 0;
    fill_count_  = 0;
}

/**
 * @brief Initialise buffer
 *
 * Note that the length is advisory only: Because of the way the
 * memory mirroring technique works, the true buffer length will
 * be multiples of the device page size (e.g. 4096 bytes)
 *
 * @param a_length Length of buffer
 */
bool osal::windows::CircularBuffer::Init (const char* a_data_path, int32_t a_length)
{
    int32_t page_size = 65536;
    Close();

    // The buffer length must be a multiple of page size
    length_ = ((a_length + page_size - 1) / page_size) * page_size;

    map_handle_ = CreateFileMappingA(INVALID_HANDLE_VALUE,
                                     NULL,
                                     PAGE_READWRITE,
                                     0,
                                     length_ * 2,
                                     "WTF");
    if ( map_handle_ == INVALID_HANDLE_VALUE ) {
        goto cleanup;
    }

    for ( int i = 0; i < 10; ++i ) {
        lower_half_ = VirtualAlloc(0, length_ * 2, MEM_RESERVE, PAGE_NOACCESS);
        if ( lower_half_ == NULL ) {
            continue;
        }
        VirtualFree(lower_half_, 0, MEM_RELEASE);
        lower_half_ = (void*) MapViewOfFileEx(map_handle_,
                                              FILE_MAP_ALL_ACCESS,
                                              0,
                                              0,
                                              length_,
                                              lower_half_);
        if ( lower_half_ == NULL ) {
            continue;
        }
        upper_half_ = (void*) MapViewOfFileEx(map_handle_,
                                              FILE_MAP_ALL_ACCESS,
                                              0,
                                              0,
                                              length_,
                                              (BYTE*) lower_half_ + length_);
        if ( upper_half_ == NULL ) {
            UnmapViewOfFile(lower_half_);
            continue;
        }

        if ( ((BYTE*) lower_half_ + length_) == (BYTE*) upper_half_ ) {
            break;
        } else {
            int err = GetLastError();
            DEBUGTRACE("CircularBuffer", "ERROR %d", GetLastError());
        }
    }
    if ( ((BYTE*) lower_half_ + length_) == (BYTE*) upper_half_ ) {
        strcpy((char*) upper_half_, "the quick brown fox jumps over the lazy dog");
        memcpy(lower_half_, "HELLO", 5);
        if ( strncmp((char*) lower_half_, "HELLOuick brown fox jumps over the lazy dog", 43) == 0 ) {
            buffer_ = lower_half_;
        }
        return true;
    }

cleanup:
    Close();
    return false;
}

/**
 * @brief Cleanup buffer
 *
 * Releases buffer resources.
 */
void osal::windows::CircularBuffer::Close ()
{
    if ( buffer_ != NULL ) {
        UnmapViewOfFile(lower_half_);
        UnmapViewOfFile(upper_half_);
        buffer_ = NULL;
    }

    if (map_handle_ != INVALID_HANDLE_VALUE ) {
        CloseHandle(map_handle_);
        map_handle_ = INVALID_HANDLE_VALUE;
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
void osal::windows::CircularBuffer::Clear ()
{
    int32_t count;

    if ( Tail(&count) != 0 ) {
        Consume(count);
    }
}

/**
 * Helper routine to copy bytes to buffer
 *
 *  This copies the given bytes to the buffer, and marks them ready for writing.
 *
 * @param a_src Source buffer
 * @param a_length Number of bytes in source buffer
 * @return true if bytes copied, false if there was insufficient space
 */
bool osal::windows::CircularBuffer::ProduceBytes (const void* a_src, int32_t a_length)
{
    int32_t space;
    void *ptr = Head(&space);
    if ( space < a_length ) {
        return false;
    } else {
        memcpy(ptr, a_src, a_length);
        Produce(a_length);
        return true;
    }
}
