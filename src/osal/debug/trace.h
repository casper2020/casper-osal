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
#include <string.h> // strerror

#include <sys/types.h>
#include <sys/stat.h> // chmod
#include <pwd.h>      // getpwuid
#include <grp.h>      // getgrgid
#include <unistd.h>   // getpid, chown

#define OSAL_DEBUG_TRACE_LOCK_GUARD() \
    std::lock_guard<std::mutex> lock(mutex_);

namespace osal
{

    namespace debug
    {

        // ---- //
        class Trace;
        class TraceOneShot final : public ::osal::Initializer<Trace>
        {
            
        public: // Constructor(s) / Destructor
            
            TraceOneShot (Trace& a_instance);
            virtual ~TraceOneShot ();
            
        }; // end of class 'TraceOneShot'    
        
        /**
         * @brief A singleton to log debug messages.
         */
        class Trace final : public osal::Singleton<Trace, TraceOneShot>
        {
            
            friend class TraceOneShot;

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

            uid_t       user_id_;
            std::string user_name_;
            gid_t       group_id_;
            std::string group_name_;
            mode_t      mode_;

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

        public: // Method(s) / Function(s)
            
            bool EnsureOwnership (uid_t a_user_id, gid_t a_group_id);

        private: // Method(s) / Function(s)
            
            bool EnsureOwnership ();
            bool EnsureOwnership (const std::string& a_uri);

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
            mode_           = ( S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH );
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
            }
            // ... write a comment line ...
            for ( auto it : tokens_ ) {
                // ... skip if not a 'real' file ...
                if ( stdout == it.second->fp_ || stderr == it.second->fp_ ) {
                    continue;
                }
                // .. ensure write permissions ...
                (void)EnsureOwnership(it.second->uri_);
                // ... log recycle messages ...
                fprintf(it.second->fp_, "--- --- ---\n");
                fprintf(it.second->fp_, "⌥ LOG FILE   : %s\n", it.second->uri_.c_str());
                fprintf(it.second->fp_, "⌥ OWNERSHIP  : %4o\n", mode_);
                if ( not ( ( UINT32_MAX == user_id_ || UINT32_MAX == group_id_ ) || ( 0 == user_id_ || 0 == group_id_ ) ) ) {
                    fprintf(it.second->fp_, "  - USER : %4" PRIu32 " - %s\n", user_id_, user_name_.c_str());
                    fprintf(it.second->fp_, "  - GROUP: %4" PRIu32 " - %s\n", group_id_, group_name_.c_str());
                }
                fprintf(it.second->fp_, "⌥ PERMISSIONS:\n");
                fprintf(it.second->fp_, "  - MODE : %-4o\n", mode_);
                fprintf(it.second->fp_, "--- --- ---\n");
                fflush(it.second->fp_);
            }
        }
        
        /**
         * @brief Output a log message if the provided token is registered.
         *
         * @param a_token The token to be tested.
         * @param a_format printf like format followed by a variable number of arguments
         * @param ... __VA_ARGS__
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
                    if ( true == EnsureBufferCapacity(static_cast<size_t>((unsigned long)aux + sizeof(char))) ) {
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
         * @param a_function Calling function.
         * @param a_line     Call file line.
         * @param a_format   printf like format
         * @param ...        prontf like arguments
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
                    if ( true == EnsureBufferCapacity(static_cast<size_t>((unsigned long)aux + sizeof(char))) ) {
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
         * @param a_user_id  Log file owner ID.
         * @param a_group_id Log file owner group ID.
         *
         * @return True if all files changed to new permissions or it not needed, false otherwise.
         */
        inline bool Trace::EnsureOwnership (uid_t a_user_id, gid_t a_group_id)
        {
            OSAL_DEBUG_TRACE_LOCK_GUARD();
            user_id_    = a_user_id;
            user_name_  = "";
            group_id_   = a_group_id;
            group_name_ = "";
            if ( not ( UINT32_MAX == user_id_ || 0 == user_id_) ) {
                struct passwd* pw = getpwuid(user_id_);
                if ( nullptr != pw ) {
                    user_name_ = pw->pw_name;
                }
            }
            if ( not ( UINT32_MAX == user_id_ || 0 == user_id_) ) {
                struct group* gr = getgrgid(group_id_);
                if ( nullptr != gr ) {
                    group_name_ = gr->gr_name;
                }
            }
            return EnsureOwnership();
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
                if ( 0 == it.second->uri_.length() || true == EnsureOwnership(it.second->uri_.c_str()) ) {
                    count++;
                }
            }
            return ( tokens_.size() == count );
        }
        
        /**
         * @brief Change the logs permissions to a specific user / group for a specific file.
         *
         * @param a_uri File URI,
         *
         * @return True if changed to new permissions or it not needed, false otherwise.
         */
        inline bool Trace::EnsureOwnership (const std::string& a_uri)
        {
            if ( ( UINT32_MAX == user_id_ || UINT32_MAX == group_id_ ) || ( 0 == user_id_ || 0 == group_id_ ) ) {
                return true;
            }
            const int chown_status = chown(a_uri.c_str(), user_id_, group_id_);
            if ( 0 != chown_status ) {
                fprintf(stderr, "WARNING: failed to change ownership of %s to %u:%u ~ %d - %s\n", a_uri.c_str(), user_id_, group_id_, errno, strerror(errno));
                fflush(stderr);
            }
            const int chmod_status = chmod(a_uri.c_str(), mode_);
            if ( 0 != chmod_status ) {
                fprintf(stderr, "WARNING: failed to change permissions of %s to %o ~ %d - %s\n", a_uri.c_str(), mode_, errno, strerror(errno));
                fflush(stderr);
            }
            return ( 0 == chown_status && 0 == chmod_status );
        }
            
    } // end of namesapce debug

} // end of namespace osal

#endif // NRS_OSAL_DEBUG_TRACE_H_
