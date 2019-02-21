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

#pragma once
#ifndef NRS_OSAL_POSIX_POSIX_DATAGRAM_SOCKET_H_
#define NRS_OSAL_POSIX_POSIX_DATAGRAM_SOCKET_H_

#include <sys/un.h>     // struct sockaddr_un
#include <sys/socket.h> // socklen_t
#include <string>       // std::string

namespace osal
{
    
    namespace posix
    {
        
        class DatagramSocket
        {
            
        protected: // Data Type(s)
            
            enum class Step : uint8_t {
                None,
                NotSet,
                Create,
                Bind,
                SendOrReceive
            };
            
        protected: // Data
            
            int                fd_;
            std::string        fn_;
            int                last_error_;
            std::string        last_error_string_;
            int                last_tx_error_;
            std::string        last_tx_error_string_;
            int                last_rx_error_;
            std::string        last_rx_error_string_;

            struct sockaddr_un dst_attr_;
            socklen_t          dst_addr_len_;

            Step               next_step_;

        public: // constructor(s) / destructor
            
            DatagramSocket ();
            virtual ~DatagramSocket();
            
        public: // Method(s) / Function(s) declaration
            
            virtual bool Create      (const std::string& a_file_name);
            virtual bool SetNonBlock ();
            virtual bool SetReuseAddr();
            virtual bool Close       ();
            virtual bool Send        (const std::string& a_message);
            virtual bool Send        (const char* const a_format, ...) __attribute__((format(printf, 2, 3)));
            virtual bool Receive     (uint8_t* a_buffer, const size_t& a_length, size_t& o_length);
            
        protected: // Method(s) / Function(s)
            
            virtual void InitializeAddr();
            
        public:
            
            const int&         GetFileDescriptor         () const;
            
            int          GetLastConfigError        () const;
            const std::string& GetLastConfigErrorString  () const;
            
            int          GetLastSendError          () const;
            const std::string& GetLastSendErrorString    () const;
            
            int          GetLastReceiveError       () const;
            const std::string& GetLastReceiveErrorString () const;
            
        }; // end of class 'DatagramSocket'
        
        inline const int& DatagramSocket::GetFileDescriptor () const
        {
            return fd_;
        }
        
        inline int DatagramSocket::GetLastConfigError () const
        {
            return last_error_;
        }
        
        inline const std::string& DatagramSocket::GetLastConfigErrorString () const
        {
            return last_error_string_;
        }

        inline int DatagramSocket::GetLastSendError () const
        {
            return last_tx_error_;
        }
        
        inline const std::string& DatagramSocket::GetLastSendErrorString () const
        {
            return last_tx_error_string_;
        }
        
        inline int DatagramSocket::GetLastReceiveError () const
        {
            return last_rx_error_;
        }
        
        inline const std::string& DatagramSocket::GetLastReceiveErrorString () const
        {
            return last_rx_error_string_;
        }

        class DatagramServerSocket : public DatagramSocket {

        public: // constructor(s) / destructor
            
            DatagramServerSocket ();
            virtual ~DatagramServerSocket();
            
        public: // Method(s) / Function(s) declaration

            virtual bool Create (const std::string& a_file_name);
            virtual bool Bind   ();

        };

        class DatagramClientSocket : public DatagramSocket {
            
        public: // constructor(s) / destructor
            
            DatagramClientSocket ();
            virtual ~DatagramClientSocket();
            
        public: // Method(s) / Function(s) declaration
            
            virtual bool Create (const std::string& a_file_name);
            virtual bool Bind   ();
        };

    } // end of namespace 'posix'
    
} // end of namespace 'osal'

#endif // NRS_OSAL_POSIX_POSIX_DATAGRAM_SOCKET_H_
