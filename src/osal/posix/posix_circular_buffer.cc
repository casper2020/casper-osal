/**
 * @file CircularBuffer.h Lockless Circular buffer - implementation
 *
 * Circular buffer without locks and continuos memory copy using two contiguous regions of virtual memory
 * this a POSIX/GCC port of the fantastic TPCircularBuffer made by Michael Tyson, original code is at
 * https://github.com/michaeltyson/TPCircularBuffer
 *
 * Adapted for POSIX / GCC by Eurico Inocencio 2012-10-28, MIT License kept
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

#include "osal/posix/posix_circular_buffer.h"
#include "osal/debug_trace.h"
#include <string.h>   // for memcpy
#include <unistd.h>   // for close, ftruncate
#include <fcntl.h>    // for open
#include <stdio.h>    // Temporary

#ifdef __ANDROID__
#undef perror
#define perror(V) DEBUGTRACE("CircularBuffer", V)
#endif

void osal::posix::CircularBuffer::Init ()
{
    lower_half_ = MAP_FAILED;
    upper_half_ = MAP_FAILED;
    buffer_     = MAP_FAILED;
    length_     = 0;
    tail_       = 0;
    head_       = 0;
    fill_count_ = 0;
    seekable_   = true;
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
bool osal::posix::CircularBuffer::Init (const char* a_data_path, int32_t a_length)
{
    char szTmp[1024];
    int32_t page_size = 4096;
    int fd = -1;

    int status;

    Close();

    // The buffer length must be a multiple of page size
    length_ = ((a_length + page_size - 1) / page_size) * page_size;

    snprintf(szTmp, sizeof(szTmp), "%s%s%p.tmp", a_data_path, a_data_path[strlen(a_data_path)-1] == '/' ? "" : "/", (void*) this);
    fd = open(szTmp, O_RDWR | O_CREAT);
    if ( fd < 1 ) {
        perror("open");
        goto cleanup;
    }
    status = unlink(szTmp);
    if (status) {
        perror("unlink");
        goto cleanup;
    }
    status = ftruncate(fd, length_);
    if (status) {
        perror("ftruncate");
        goto cleanup;
    }
    buffer_ = mmap(NULL, static_cast<size_t>(length_ * 2), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (buffer_ == MAP_FAILED) {
        perror("map anon");
        goto cleanup;
    }
    lower_half_ = mmap(buffer_, static_cast<size_t>(length_), PROT_READ | PROT_WRITE, MAP_FIXED | MAP_SHARED, fd, 0);
    if ( lower_half_ == MAP_FAILED) {
        perror("map lower");
        goto cleanup;
    }
    upper_half_ = mmap((int8_t*) buffer_ + length_, static_cast<size_t>(length_), PROT_READ | PROT_WRITE, MAP_FIXED | MAP_SHARED, fd, 0);
    if ( upper_half_ == MAP_FAILED ) {
        perror("map upper");
        goto cleanup;
    }
    status = close(fd);
    if ( status == 0 ) {
        head_ = 0;
        tail_ = 0;
        fill_count_ = 0;
        if ( /* DISABLES CODE */ (false) ) {
            // Uncoment this code to test the wrap arround
            strcpy((char*) upper_half_, "the quick brown fox jumps over the lazy dog");
            DEBUGTRACE("CircularBuffer", "%s\n", (char*)lower_half_);
            memcpy(lower_half_, "HELLO", 5);
            DEBUGTRACE("CircularBuffer", "%s\n", (char*)upper_half_);
        }
        return true;
    }
 cleanup:
    close(fd);
    Close();
    return false;
}

/**
 * @brief Cleanup buffer
 *
 * Releases buffer resources.
 */
void osal::posix::CircularBuffer::Close ()
{
    if ( length_ != 0 ) {
        if ( lower_half_ != MAP_FAILED ) {
            munmap(lower_half_, static_cast<size_t>(length_));
        }
        if ( upper_half_ != MAP_FAILED ) {
            munmap(upper_half_, static_cast<size_t>(length_));
        }
        if ( buffer_ != MAP_FAILED ) {
            munmap(buffer_, static_cast<size_t>(length_) * 2);
        }
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
void osal::posix::CircularBuffer::Clear ()
{
    int32_t count;
    if ( Tail(&count) != 0 ) {
        Consume(count);
    }
    seekable_ = true;
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
bool osal::posix::CircularBuffer::ProduceBytes (const void* a_src, int32_t a_length)
{
    int32_t space;
    void *ptr = Head(&space);
    if ( space < a_length ) {
        return false;
    } else {
        memcpy(ptr, a_src, static_cast<size_t>(a_length));
        Produce(a_length);
        return true;
    }
}


