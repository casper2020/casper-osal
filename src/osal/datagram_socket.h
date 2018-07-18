/**
 * @file datagram_socket.h
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

#pragma once
#ifndef NRS_OSAL_DATAGRAM_SOCKET_H_
#define NRS_OSAL_DATAGRAM_SOCKET_H_

#include "osal/posix/posix_datagram_socket.h"

namespace osal
{
    typedef osal::posix::DatagramSocket	DatagramSocket;
    typedef osal::posix::DatagramServerSocket DatagramServerSocket;
    typedef osal::posix::DatagramClientSocket DatagramClientSocket;
}

#endif // NRS_OSAL_DATAGRAM_SOCKET_H_
