#pragma once
/**
 * @file trace.h - Debug trace.
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

#ifndef NRS_OSAL_DEBUG_TRACE_H_
#define NRS_OSAL_DEBUG_TRACE_H_

#include "osal/osal_singleton.h"

#include <exception>
#include <string>
#include <map>
#include <limits.h>
#include <stdarg.h>
#include <mutex> // std::mutext, std::lock_guard

#include <sys/types.h>
#include <sys/stat.h> // chmod
#include <unistd.h> // getpid, chown

#define OSAL_DEBUG_TRACE_LOCK_GUARD() \
    std::lock_guard<std::mutex> lock(mutex_);

namespace osal
{

    namespace debug
    {

        /**
         * @brief A singleton to log debug messages.
         */
        class Trace final : public osal::Singleton<Trace>
        {

        public: // Data Type(s)
              
              class RegistrationException : public std::exception {
                  
              private: // Data
                  
                  const std::string what_;
                  
              public: // Constructor / Destructor
                  
                  RegistrationException (const std::string& a_what)
                    : what_(a_what)
                  {
                      // ... empty ...
                  }
                  
                  virtual ~RegistrationException ()
                  {
                      // ... empty ...
                  }
                  
              public:
                  
                  const char* what() const noexcept {return what_.c_str();}
                  
              };
          
        private: // Static Data
            
            std::mutex mutex_;

        protected: // Data Types

            /**
             * An object that defines a token.
             */
            class Token
            {

            public: // Const Data

                const std::string name_;
                const std::string uri_;

            public: // Data

                FILE* fp_;

            public: // Constructor(s) / Destructor

                /**
                 * @brief Default constructor.
                 *
                 * @param a_name The token name.
                 */
                Token (const std::string& a_name, const std::string& a_uri, FILE* a_file)
                : name_(a_name), uri_(a_uri), fp_(a_file)
                {
                    /* empty */
                }

                /**
                 * @brief Destructor.
                 */
                virtual ~Token ()
                {
                    /* empty */
                }

            };

        private: // Data

            std::map<std::string, Token*> tokens_;
            char*                 buffer_;
            size_t                buffer_capacity_;
            
        private: // Data

            uid_t user_id_;
            gid_t group_id_;

        public: // Initialization / Release API - Method(s) / Function(s)

            void    Startup  ();
            void    Shutdown ();

        public: // Registration API - Method(s) / Function(s)

            void     Register     (const std::string& a_token, FILE* a_file);
            void     Register     (const std::string& a_token, const std::string& a_file);
            bool     IsRegistered (const char* const a_token);
            void     Recycle      ();

        public: // Log API - Method(s) / Function(s)

            void     Log         (const std::string& a_token, const char* a_format, ...);
            void     LogExtended (const std::string& a_token, const char* a_function, const int& a_line, const char* a_format, ...);

        private: //

            bool     EnsureBufferCapacity (const size_t& a_capacity);

        private: // Method(s) / Function(s)
            
            bool EnsureOwnership ();

        public: // Method(s) / Function(s)
            
            bool EnsureOwnership (uid_t a_user_id, gid_t a_group_id);

        }; // end of class Trace

        /**
         * @brief Initialize trace instance.
         */
        inline void Trace::Startup  ()
        {
            OSAL_DEBUG_TRACE_LOCK_GUARD();
            buffer_          = new char[1024];
            buffer_capacity_ = nullptr != buffer_ ? 1024 : 0;
            user_id_         = UINT32_MAX;
            group_id_        = UINT32_MAX;

        }

        /**
         * @brief Release all dynamically allocated memory
         */
        inline void Trace::Shutdown ()
        {
            OSAL_DEBUG_TRACE_LOCK_GUARD();
            for ( auto it : tokens_ ) {
                delete it.second;
            }
            tokens_.clear();
            if ( nullptr != buffer_ ) {
                delete [] buffer_;
            }
        }

        /**
         * @brief Register a token.
         *
         * @param a_token The token name.
         */
        inline void Trace::Register (const std::string& a_token, FILE* a_file)
        {
            OSAL_DEBUG_TRACE_LOCK_GUARD();
            // ... invalid output? ...
            if ( nullptr == a_file ) {
                // ... yes ...
                return;
            }
            // ... already registered? ...
            if ( tokens_.end() != tokens_.find(a_token) ) {
                // ... yes ..
                return;
            }
            // ... keep track of it ...
            tokens_[a_token] = new Token(a_token, "", a_file);
        }

        /**
         * @brief Register a token.
         *
         * @param a_token The token name.
         */
        inline void Trace::Register (const std::string& a_token, const std::string& a_uri)
        {
            OSAL_DEBUG_TRACE_LOCK_GUARD();
            // ... already registered? ...
            if ( tokens_.end() != tokens_.find(a_token) ) {
                // ... yes ..
                return;
            }
            // ... open file and keep track of it ...
            FILE* fp = fopen(a_uri.c_str(), "a");
            if ( nullptr != fp ) {
                tokens_[a_token] = new Token(a_token, a_uri, fp);
            }
        }
        
        /**
         * @brief Check if a token is already registered.
         *
         * @param a_token The token name.
         *
         * @return True when the token is already registered.
         */
        inline bool Trace::IsRegistered (const char* const a_token)
        {
            OSAL_DEBUG_TRACE_LOCK_GUARD();
            return tokens_.end() != tokens_.find(a_token);
        }
    
        /**
         * @brief Re-open log files ( if any ).
         */
        inline void Trace::Recycle ()
        {
            OSAL_DEBUG_TRACE_LOCK_GUARD();
            // ... for all tokens ...
            for ( auto it : tokens_ ) {
                // ... if no file is open ...
                if ( stdout == it.second->fp_ || stderr == it.second->fp_ ) {
                    // ... next ...
                    continue;
                }
                // ... else close ...
                if ( nullptr != it.second->fp_ ) {
                    fclose(it.second->fp_);
                }
                // ... and reopen it ...
                it.second->fp_ = fopen(it.second->uri_.c_str(), "w");
                if ( nullptr == it.second->fp_ ) {
                    const char* const err_str = strerror(errno);
                    throw RegistrationException(
                        "An error occurred while creating rotating log file '" + it.second->uri_ + "': " + std::string(nullptr != err_str ? err_str : "nullptr") + " !"
                    );
                }
                // ... write a comment line ...
                fprintf(it.second->fp_, "---- NEW LOG '%s' ----\n", it.second->uri_.c_str());
                fflush(it.second->fp_);
            }
            EnsureOwnership();
        }
        
        /**
         * @brief Output a log message if the provided token is registered.
         *
         * @param a_token The token to be tested.
         * @param a_format
         * @param ...
         */
        inline void Trace::Log (const std::string& a_token, const char* a_format, ...)
        {
            OSAL_DEBUG_TRACE_LOCK_GUARD();
            // ...if token is not registered...
            if ( tokens_.end() == tokens_.find(a_token) ) {
                // ... we're done ...
                return;
            }

            // ... ensure we have a valid buffer ...
            if ( false == EnsureBufferCapacity(1024) ) {
                // ... oops ...
                return;
            }

            int aux = INT_MAX;

            // ... try at least 2 times to construct the output message ...
            for ( uint8_t attempt = 0 ; attempt < 2 ; ++attempt ) {

                va_list args;
                va_start(args, a_format);
                aux = vsnprintf(buffer_, buffer_capacity_, a_format, args);
                va_end(args);

                if ( aux < 0 ) {
                    // ... an error has occurred ...
                    break;
                } else if ( aux > static_cast<int>(buffer_capacity_) ) {
                    // ... realloc buffer ...
                    if ( true == EnsureBufferCapacity(static_cast<size_t>(aux + sizeof(char))) ) {
                        // ... last attempt to write to buffer ...
                        continue;
                    } else {
                        // ... out of memory ...
                        break;
                    }
                } else {
                    // ... all done ...
                    break;
                }
            }

            // ... ready to output the message ? ...
            if ( aux > 0 && static_cast<size_t>(aux) < buffer_capacity_ ) {
                auto file = tokens_.find(a_token)->second->fp_;
                // ... output message ...
                fprintf(tokens_.find(a_token)->second->fp_, "%s", buffer_);
                // ... flush ...
                if ( stdout != file && stderr != file ) {
                    fflush(file);
                }
            }
        }

        /**
         * @brief Output a log message if the provided token is registered.
         *
         * @param a_token The token to be tested.
         * @param a_function
         * @param a_line
         * @param a_format
         * @param ...
         */
        inline void Trace::LogExtended (const std::string& a_token, const char* a_function, const int& a_line, const char* a_format, ...)
        {
            OSAL_DEBUG_TRACE_LOCK_GUARD();
            // ...if token is not registered...
            if ( tokens_.end() == tokens_.find(a_token) ) {
                // ... we're done ...
                return;
            }

            // ... ensure we have a valid buffer ...
            if ( false == EnsureBufferCapacity(1024) ) {
                // ... oops ...
                return;
            }

            int aux = INT_MAX;

            // ... try at least 2 times to construct the output message ...
            for ( uint8_t attempt = 0 ; attempt < 2 ; ++attempt ) {

                va_list args;
                va_start(args, a_format);
                aux = vsnprintf(buffer_, buffer_capacity_, a_format, args);
                va_end(args);

                if ( aux < 0 ) {
                    // ... an error has occurred ...
                    break;
                } else if ( aux > static_cast<int>(buffer_capacity_) ) {
                    // ... realloc buffer ...
                    if ( true == EnsureBufferCapacity(static_cast<size_t>(aux + sizeof(char))) ) {
                        // ... last attempt to write to buffer ...
                        continue;
                    } else {
                        // ... out of memory ...
                        break;
                    }
                } else {
                    // ... all done ...
                    break;
                }
            }

            // ... ready to output the message ? ...
            if ( aux > 0 && static_cast<size_t>(aux) < buffer_capacity_ ) {
                auto file = tokens_.find(a_token)->second->fp_;
                // ... output message ...
                if ( nullptr != a_function ) {
                    fprintf(file, "\n[%s] @ %s : %d\n",
                            a_token.c_str(), a_function, a_line);
                    // ... output message ...
                    fprintf(file, "\n\t* %s\n", buffer_);
                } else {
                    // ... output message ...
                    fprintf(file, "\n%s\n", buffer_);
                }
                // ... flush ...
                if ( stdout != file && stderr != file ) {
                    fflush(file);
                }
            }
        }
        
        /**
         * @brief Ensure that the current buffer has at least the required capacity.
         *
         * @param a_capacity The required capacity ( in bytes ).
         *
         * @return True when the buffer has at least the required capacity.
         */
        inline bool Trace::EnsureBufferCapacity (const size_t& a_capacity)
        {
            // ... buffer is allocated and has enough capacity? ...
            if ( buffer_capacity_ >= a_capacity ) {
                // ... good to go ...
                return true;
            }
            // ... if buffer is not allocated ...
            if ( nullptr != buffer_ ) {
                delete [] buffer_;
            }
            // ... try to allocate it now ...
            buffer_ = new char[a_capacity];
            if ( nullptr == buffer_ ) {
                // ... out of memory ...
                buffer_capacity_ = 0;
            } else {
                // ... we're good to go ....
                buffer_capacity_ = a_capacity;
            }
            // ... we're good to go if ...
            return buffer_capacity_ == a_capacity;
        }
    
        /**
         * @brief Change the logs permissions to a specific user / group.
         *
         * @return True if all files changed to new permissions or it not needed, false otherwise.
         */
        inline bool Trace::EnsureOwnership ()
        {
            if ( UINT32_MAX == user_id_ || UINT32_MAX == group_id_ ) {
                return true;
            }
            size_t count = 0;
            for ( auto it : tokens_ ) {
                const int chown_status = chown(it.second->uri_.c_str(), user_id_, group_id_);
                const int chmod_status = chmod(it.second->uri_.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
                if ( 0 == chown_status && 0 == chmod_status ) {
                    count++;
                }
            }
            return ( tokens_.size() == count );
        }
        
        /**
         * @brief Change the logs permissions to a specific user / group.
         *
         * @param a_user_id
         * @param a_group_id
         *
         * @return True if all files changed to new permissions or it not needed, false otherwise.
         */
        inline bool Trace::EnsureOwnership (uid_t a_user_id, gid_t a_group_id)
        {
            OSAL_DEBUG_TRACE_LOCK_GUARD();
            user_id_  = a_user_id;
            group_id_ = a_group_id;
            return EnsureOwnership();
        }
        

    } // end of namesapce debug

} // end of namespace osal

#endif // NRS_OSAL_DEBUG_TRACE_H_
