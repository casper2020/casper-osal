/**
 * @file posix_datagram_socket.cc - posix datagram socket
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

#include "osal/posix/posix_datagram_socket.h"

#include <errno.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <stddef.h>
#include <unistd.h>     // close fd
#include <cstdarg>      // va_start, va_end, std::va_list
#include <vector>       // std::vector
#include <fcntl.h>      // fcntl
#include <limits>       // std::numeric_limits
#include <stdexcept>    // std::runtime_error

/**
 * @brief Constructor.
 */
osal::posix::DatagramSocket::DatagramSocket ()
{
    fd_               = -1;
    last_error_       = 0;
    last_tx_error_    = 0;
    last_rx_error_    = 0;
    dst_addr_len_     = std::numeric_limits<socklen_t>::max();
    next_step_        = osal::posix::DatagramSocket::Step::Create;
}

/**
 * @brief Destructor.
 */
osal::posix::DatagramSocket::~DatagramSocket ()
{
    if ( -1 != fd_ ) {
        close(fd_);
    }
}

/**
 * @brief Create an endpoint for communication.
 *
 * @param a_file_name
 *
 * @return
 */
bool osal::posix::DatagramSocket::Create (const std::string& a_file_name)
{
    if ( osal::posix::DatagramSocket::Step::Create != next_step_ ) {
        return false;
    }
    
    fn_ = a_file_name;

    fd_ = socket(AF_UNIX, SOCK_DGRAM, 0);
    if ( fd_ < 0 ) {
        last_error_        = errno;
        last_error_string_ = strerror(last_error_);
    } else {
        // For F_SETFL, 0 must be returned on success
        // On error, -1 is returned, and errno is set appropriately
        last_error_        = 0;
        last_error_string_ = "";
        next_step_         = osal::posix::DatagramSocket::Step::NotSet;
    }
    
    return 0 == last_error_;
}

/**
 * @brief Set the O_ASYNC file status flag on the file descriptor.
 *
 * @return
 */
bool osal::posix::DatagramSocket::SetNonBlock ()
{
    if ( -1 == fd_ ) {
        return false;
    }
    
    if ( -1 == fcntl(fd_, F_SETFL, O_NONBLOCK) ) {
        last_error_        = errno;
        last_error_string_ = strerror(last_error_);
    } else {
        last_error_        = 0;
        last_error_string_ = "";
    }
    
    return 0 == last_error_;
}

/**
 * @brief Set reuse addr.
 *
 * @return
 */
bool osal::posix::DatagramSocket::SetReuseAddr ()
{
	if ( -1 == fd_ ) {
        return false;
    }

	int reuseaddr = 1;
	if ( -1 == setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, (const void *)&reuseaddr, sizeof(int)) ) {
        last_error_        = errno;
        last_error_string_ = strerror(last_error_);
    } else {
        last_error_        = 0;
        last_error_string_ = "";
    }

	return 0 == last_error_;
}

/**
 * @brief Close the currenlty open endpoint for communication.
 *
 * @return
 */
bool osal::posix::DatagramSocket::Close ()
{
    if ( not ( osal::posix::DatagramSocket::Step::Bind == next_step_ || osal::posix::DatagramSocket::Step::SendOrReceive == next_step_ ) ) {
        return false;
    }
    
    if ( close(fd_) < 0 ) {
        last_error_        = errno;
        last_error_string_ = strerror(last_error_);
        return false;
    }
    
    fd_ = -1;
    return true;
}

/**
 * @brief Send a message from the currently connected socket.
 *
 * @param a_message
 *
 * @return
 */
bool osal::posix::DatagramSocket::Send (const std::string& a_message)
{
    if ( osal::posix::DatagramSocket::Step::SendOrReceive != next_step_ ) {
        return false;
    }
    
    if ( 0 == a_message.length() ) {
        return false;
    }
    
    const ssize_t sent_bytes = sendto(fd_, a_message.c_str(), a_message.length(), 0, (struct sockaddr *)&dst_attr_, dst_addr_len_);
    if ( sent_bytes < 0 ) {
        last_tx_error_        = errno;
        last_tx_error_string_ = strerror(last_tx_error_);
    } else {
        last_tx_error_        = 0;
        last_tx_error_string_ = "";
    }

    return 0 == last_tx_error_;
}

/**
 * @brief Send a message from the currently connected socket.
 *
 * @param a_format
 * @param ...
 *
 * @return
 */
bool osal::posix::DatagramSocket::Send (const char* const a_format, ...)
{
    if ( osal::posix::DatagramSocket::Step::SendOrReceive != next_step_ ) {
        return false;
    }
    
    auto temp   = std::vector<char> {};
    auto length = std::size_t { 512 };
    std::va_list args;
    while ( temp.size() <= length ) {
        temp.resize(length + 1);
        va_start(args, a_format);
        const auto status = std::vsnprintf(temp.data(), temp.size(), a_format, args);
        va_end(args);
        if ( status < 0 ) {
            throw std::runtime_error {"string formatting error"};
        }
        length = static_cast<std::size_t>(status);
    }
    if ( 0 == length ) {
		return false;
	}
    return Send(std::string { temp.data(), length });
}

/**
 * @brief Receive a message from the currently connected socket.
 *
 * @param a_buffer
 * @param a_length
 * @param o_length
 *
 * @return
 */
bool osal::posix::DatagramSocket::Receive (uint8_t* a_buffer, const size_t& a_length, size_t& o_length)
{
    if ( osal::posix::DatagramSocket::Step::SendOrReceive != next_step_ ) {
        return false;
    }
    
    if ( nullptr == a_buffer ) {
        return false;
    }

    o_length = 0;

    ssize_t received_bytes = recvfrom(fd_, a_buffer, a_length,
									  /* flags */ 0, /* address */  nullptr, /* address length */ 0);
	if ( received_bytes < 0 ) {
        last_rx_error_        = errno;
        last_rx_error_string_ = strerror(last_rx_error_);
        o_length              = 0;
    } else {
        last_rx_error_        = 0;
        last_rx_error_string_ = "";
        o_length              = static_cast<size_t>(received_bytes);
    }

    return 0 == last_rx_error_;
}

/**
 * @brief Initialize socket address struct.
 */
void osal::posix::DatagramSocket::InitializeAddr ()
{
    const size_t max_len = sizeof(dst_attr_.sun_path);
    /*
     * For portability clear the whole structure, since some
     * implementations have additional (nonstandard) fields in
     * the structure.
     */
    memset(&dst_attr_, 0, sizeof(struct sockaddr_un));
    
    dst_attr_.sun_family = AF_UNIX;
    strncpy(dst_attr_.sun_path, fn_.c_str(), max_len);
    dst_attr_.sun_path[max_len - 1] = '\0';
    dst_addr_len_ = (offsetof(struct sockaddr_un, sun_path) + static_cast<socklen_t>(strlen(dst_attr_.sun_path)));
}

#ifdef __APPLE__
#pragma mark DatagramServerSocket
#endif

/**
 * @brief Default constructor.
 */
osal::posix::DatagramServerSocket::DatagramServerSocket ()
{
    /* empty */
}

/**
 * @brief Destructor.
 */
osal::posix::DatagramServerSocket::~DatagramServerSocket ()
{
    unlink(fn_.c_str());
}

/**
 * @brief Create an endpoint for communication.
 *
 * @param a_file_name
 *
 * @return
 */
bool osal::posix::DatagramServerSocket::Create (const std::string& a_file_name)
{
    if ( false == osal::posix::DatagramSocket::Create(a_file_name) ) {
        return false;
    }
    next_step_ = osal::posix::DatagramSocket::Step::Bind;
    return true;
}

/**
 * @brief Bind a name to a socket.
 *
 * @return
 */
bool osal::posix::DatagramServerSocket::Bind ()
{
    if ( osal::posix::DatagramSocket::Step::Bind != next_step_ ) {
        return false;
    }

    InitializeAddr();

    if ( bind(fd_, (struct sockaddr *)&dst_attr_, dst_addr_len_) < 0 ) {
        last_error_        = errno;
        last_error_string_ = strerror(last_error_);
    } else {
        last_error_        = 0;
        last_error_string_ = "";
        next_step_         = osal::posix::DatagramSocket::Step::SendOrReceive;
    }
    
    return 0 == last_error_;
}


#ifdef __APPLE__
#pragma mark DatagramClientSocket
#endif

/**
 * @brief Default constructor.
 */
osal::posix::DatagramClientSocket::DatagramClientSocket ()
{
    /* empty */
}

/**
 * @brief Destructor.
 */
osal::posix::DatagramClientSocket::~DatagramClientSocket ()
{
    /* empty */
}

/**
 * @brief Create an endpoint for communication.
 *
 * @param a_file_name
 *
 * @return
 */
bool osal::posix::DatagramClientSocket::Create (const std::string& a_file_name)
{
    if ( false == osal::posix::DatagramSocket::Create(a_file_name) ) {
        return false;
    }
    next_step_ = osal::posix::DatagramSocket::Step::Bind;
    return true;
}

/**
 * @brief Bind a name to a socket.
 *
 * @return
 */
bool osal::posix::DatagramClientSocket::Bind (const bool a_fake)
{
    if ( osal::posix::DatagramSocket::Step::Bind != next_step_ ) {
        return false;
    }
    
    InitializeAddr();
    
    if ( false == a_fake && 0 == last_error_ ) {
        if ( bind(fd_, (struct sockaddr *)&dst_attr_, dst_addr_len_) < 0 ) {
            last_error_        = errno;
            last_error_string_ = strerror(last_error_);
        } else {
            last_error_        = 0;
            last_error_string_ = "";
            next_step_         = osal::posix::DatagramSocket::Step::SendOrReceive;
        }
    } else if ( true == a_fake && 0 == last_error_ ) {
        next_step_ = osal::posix::DatagramSocket::Step::SendOrReceive;
    }

    return 0 == last_error_;
}
