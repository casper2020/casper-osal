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
            
        private: // Static Data
            
            std::mutex mutex_;

        public: // Data Types

            struct OutOfMemoryException : std::exception {
                const char* what() const noexcept {return "Out Of Memory!";}
            };

        protected: // Data Types

            /**
             * An object that defines a token.
             */
            class Token
            {

            public: // Const Data

                const std::string name_;

            public: // Data

                FILE* file_;

            public: // Constructor(s) / Destructor

                /**
                 * @brief Default constructor.
                 *
                 * @param a_name The token name.
                 */
                Token (const std::string& a_name, FILE* a_file)
                : name_(a_name), file_(a_file)
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

        public: // Initialization / Release API - Method(s) / Function(s)

            void    Startup  ();
            void    Shutdown ();

        public: // Registration API - Method(s) / Function(s)

            void     Register     (const std::string& a_token, FILE* a_file);
            void     Register     (const std::string& a_token, const std::string& a_file);
            bool     IsRegistered (const char* const a_token);

        public: // Log API - Method(s) / Function(s)

            void     Log         (const std::string& a_token, const char* a_format, ...);
            void     LogExtended (const std::string& a_token, const char* a_function, const int& a_line, const char* a_format, ...);

        private: //

            bool     IsRegistered (const std::string& a_token) const;
            bool     EnsureBufferCapacity (const size_t& a_capacity);

        }; // end of class Trace

        /**
         * @brief Initialize trace instance.
         */
        inline void Trace::Startup  ()
        {
            OSAL_DEBUG_TRACE_LOCK_GUARD();
            buffer_          = new char[1024];
            buffer_capacity_ = nullptr != buffer_ ? 1024 : 0;
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
            if ( true == IsRegistered(a_token) ) {
                // ... yes ..
                return;
            }
            // ... try create it ...
            Token* token = new Token(a_token, a_file);
            if ( nullptr != token ) {
                // ... keep track of it ...
                tokens_[a_token] = token;
            } else {
                // ... failure ...
                throw OutOfMemoryException();
            }
        }

        /**
         * @brief Register a token.
         *
         * @param a_token The token name.
         */
        inline void Trace::Register (const std::string& a_token, const std::string& a_file)
        {
            FILE* f = fopen(a_file.c_str(), "a");
            if ( nullptr != f ) {
                try {
                    Register(a_token, f);
                } catch (const std::exception& a_exception) {
                    fclose(f);
                }
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
            if ( false == IsRegistered(a_token) ) {
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
                aux = vsnprintf(buffer_, buffer_capacity_ - 1, a_format, args);
                va_end(args);

                if ( aux < 0 ) {
                    // ... an error has occurred ...
                    break;
                } else if ( aux > static_cast<int>(buffer_capacity_) ) {
                    // ... realloc buffer ...
                    if ( true == EnsureBufferCapacity(static_cast<size_t>(aux + 1)) ) {
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
                auto file = tokens_.find(a_token)->second->file_;
                // ... output message ...
                fprintf(tokens_.find(a_token)->second->file_, "%s", buffer_);
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
            if ( false == IsRegistered(a_token) ) {
                // ... we're done ...
                return;
            }

            // ...if token is not registered...
            if ( false == IsRegistered(a_token) ) {
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
                aux = vsnprintf(buffer_, buffer_capacity_ - 1, a_format, args);
                va_end(args);

                if ( aux < 0 ) {
                    // ... an error has occurred ...
                    break;
                } else if ( aux > static_cast<int>(buffer_capacity_) ) {
                    // ... realloc buffer ...
                    if ( true == EnsureBufferCapacity(static_cast<size_t>(aux + 1)) ) {
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
                auto file = tokens_.find(a_token)->second->file_;
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
         * @brief Check if a token is already registered.
         *
         * @param a_token The token name.
         *
         * @return True when the token is already registered.
         */
        inline bool Trace::IsRegistered (const std::string& a_token) const
        {
            return tokens_.end() != tokens_.find(a_token);
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

    } // end of namesapce debug

} // end of namespace osal

#endif // NRS_OSAL_DEBUG_TRACE_H_
