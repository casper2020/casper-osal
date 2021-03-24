#pragma once
/**
 * @file trace_time.h - Debug time trace.
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

#ifndef NRS_OSAL_DEBUG_TIME_TRACE_H_
#define NRS_OSAL_DEBUG_TIME_TRACE_H_

#include <exception>
#include <string>
#include <map>
#include <limits.h>
#include <stdarg.h>

#include <vector>
#include <chrono>
#include <functional>

#include "osal/osal_time.h"

namespace osal
{

    namespace debug
    {

        /**
         * @brief A singleton to log debug messages.
         */
        class TraceTime
        {

        public: // Data Types

            struct OutOfMemoryException : std::exception {
                const char* what() const noexcept {return "Out Of Memory!";}
            };

            struct OutOfBoundsException : std::exception {
                const char* what() const noexcept {return "Out Of Bounds!";}
            };

            typedef std::chrono::steady_clock::time_point TimePoint;
            typedef std::function<void(TimePoint a_start, TimePoint a_finish)> TimeTrackCallback;

        protected: // Data Types

            /**
             * An object that defines a token.
             */
            class Token
            {

            public: // Data Type(s)

                typedef struct _TimeStat {
                    TimePoint start_;
                    TimePoint finish_;
                } TimeStat;

            public: // Const Data

                const std::string name_;

            public: // Data

                FILE*                 file_;
                std::string           title_;
                std::vector<TimeStat> times_;
                size_t                count_;

            public: // Constructor(s) / Destructor

                /**
                 * @brief Default constructor.
                 *
                 * @param a_name The token name.
                 * @param a_file
                 */
                Token (const std::string& a_name, FILE* a_file)
                : name_(a_name), file_(a_file), title_(""), count_(0)
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
            void*                 parent_;

        public: // Constructor(s) / Destructor

            /**
             * @brief Constructor.
             */
            TraceTime (void* a_parent)
                : parent_(a_parent)
            {
                buffer_          = new char[1024];
                if ( nullptr != buffer_ ) {
                    buffer_[0]       = '\0';
                    buffer_capacity_ = 1024;
                } else {
                    buffer_capacity_ = 0;
                }
            }

            /**
             * @brief Destructor.
             */
            virtual ~TraceTime ()
            {
                for ( auto it : tokens_ ) {
                    delete it.second;
                }
                tokens_.clear();
                if ( nullptr != buffer_ ) {
                    delete [] buffer_;
                }
            }

        public: // Registration API - Method(s) / Function(s)

            void     Register            (const std::string& a_key, const std::string& a_name, FILE* a_file);
            void     Unregister          (const std::string& a_key);

        public: // Log API - Method(s) / Function(s)

            void         Start           (const std::string& a_key);
            void         Start           (const std::string& a_key, const char* const a_format, ...) __attribute__((format(printf, 3, 4)));
            void         Stop            (const std::string& a_key, uint8_t a_timer);
            void         Stop            (const std::string& a_key, uint8_t a_timer, TimePoint a_start, TimePoint a_finish);
            void         SetCounter      (const std::string& a_key, size_t a_value);
            void         Flush           (const std::string& a_key, const char* const a_format, ...) __attribute__((format(printf, 3, 4)));
            std::string  MakeKey         (const char* const a_format, ...);

        private: //

            void    ResetToken           (const std::string& a_key, const char* const a_title);
            void    EnsureBufferCapacity (const size_t& a_capacity);
            void    Log                  (const std::string& a_key, const char* const a_format, ...) __attribute__((format(printf, 3, 4)));
            void    Log                  (const std::string& a_key, const char* const a_format, va_list& a_args);
            int     Write                (const char* const a_format, va_list& a_args);

        }; // end of class Trace

        /**
         * @brief Register a token.
         *
         * @param a_key
         * @param a_file
         */
        inline void TraceTime::Register (const std::string& a_key, const std::string& a_name, FILE* a_file)
        {
            // ... invalid output? ...
            if ( nullptr == a_file ) {
                // ... yes ...
                return;
            }
            // ... already registered? ...
            if ( tokens_.end() != tokens_.find(a_key) ) {
                // ... yes ..
                return;
            }
            // ... try create it ...
            Token* token = new Token(a_name, a_file);
            if ( nullptr != token ) {
                // ... keep track of it ...
                tokens_[a_key] = token;
            } else {
                // ... failure ...
                throw OutOfMemoryException();
            }
            ResetToken(a_key, "");
        }

        /**
         * @brief Unregister a token.
         *
         * @param a_key
         * @param a_file
         */
        inline void TraceTime::Unregister (const std::string& a_key)
        {
            auto it = tokens_.find(a_key);
            // ... already registered? ...
            if ( tokens_.end() != it ) {
                delete it->second;
                tokens_.erase(it);
            }
        }

        /**
         * @brief Start tracking time for a specific token.
         *
         * @param a_key
         * @param a_format
         * @param ...
         */
        inline void TraceTime::Start (const std::string& a_key)
        {
            auto token = tokens_.find(a_key);
            if ( tokens_.end() == token ) {
                return;
            }

            ResetToken(a_key, "");
        }

        /**
         * @brief Start tracking time for a specific token.
         *
         * @param a_key
         * @param a_format
         * @param ...
         */
        inline void TraceTime::Start (const std::string& a_key, const char* const a_format, ...)
        {
            auto token = tokens_.find(a_key);
            if ( tokens_.end() == token ) {
                return;
            }

            va_list args;
            try {
                va_start(args, a_format);
                Write(a_format, args);
                ResetToken(a_key, buffer_);
                va_end(args);
            } catch (std::exception& a_exception) {
                va_end(args);
                throw a_exception;
            }
        }

        /**
         * @brief Stop tracking time for a specific token.
         *
         * @param a_key
         * @param a_timer
         */
        inline void TraceTime::Stop (const std::string& a_key, uint8_t a_timer)
        {
            auto token = tokens_.find(a_key);
            if ( tokens_.end() == token ) {
                return;
            }

            if ( a_timer > ( token->second->times_.size() - 1 ) ) {
                throw OutOfBoundsException();
            }
            token->second->times_[a_timer].finish_ = std::chrono::steady_clock::now();
        }

        /**
         * @brief Stop tracking time for a specific token.
         *
         * @param a_key
         * @param a_timer
         * @param a_start
         * @param a_finish
         */
        inline void TraceTime::Stop (const std::string& a_key, uint8_t a_timer,
                                       TimePoint a_start, TimePoint a_finish)
        {
            auto token = tokens_.find(a_key);
            if ( tokens_.end() == token ) {
                return;
            }

            if ( a_timer > ( token->second->times_.size() - 1 ) ) {
                throw OutOfBoundsException();
            }
            token->second->times_[a_timer].start_  = a_start;
            token->second->times_[a_timer].finish_ = a_finish;
        }

        /**
         * @brief Set a generic counter value.
         *
         * @param a_key
         * @param a_value The counter value.
         */
        inline void TraceTime::SetCounter (const std::string& a_key, size_t a_value)
        {
            auto token = tokens_.find(a_key);
            if ( tokens_.end() == token ) {
                return;
            }
            token->second->count_ = a_value;
        }

        /**
         * @brief Flush tracking time for a specific token.
         *
         * @param a_key
         * @param a_format
         * @param ...
         */
        inline void TraceTime::Flush (const std::string& a_key, const char* const a_format, ...)
        {
            auto token = tokens_.find(a_key);
            if ( tokens_.end() == token ) {
                return;
            }

            const auto elapsed_one = std::chrono::duration_cast<std::chrono::milliseconds>(token->second->times_[0].finish_ - token->second->times_[0].start_).count();
            const auto elapsed_two = std::chrono::duration_cast<std::chrono::milliseconds>(token->second->times_[1].finish_ - token->second->times_[1].start_).count();

            osal::Time::HumanReadableTime h_r_time;
            osal::Time::GetHumanReadableTimeFromUTC(osal::Time::GetUTC(), h_r_time);

            //  data  addr           e1     e2  msg  ...
            //    0xffffffff, 00000, 00000, tlt, ...
            //  "[%02d/%3s/%04d:%02d:%02d:%02d +0000], %p, %5.5f, %5.5f, %s, %s";
            Log(a_key, "[%02d/%3s/%04d:%02d:%02d:%02d +0000], %p, %05lld, %05lld, %3d, %8s, %s, ",
                (int)h_r_time.day_, osal::Time::GetHumanReadableMonth(h_r_time.month_),  (int)h_r_time.year_,  (int)h_r_time.hours_,  (int)h_r_time.minutes_,  (int)h_r_time.seconds_,
                         parent_, elapsed_one, elapsed_two, static_cast<int>(token->second->count_), token->second->name_.c_str(), token->second->title_.c_str());

            token->second->count_  = 0;

            // write status
            va_list args;
            va_start(args, a_format);
            Log(a_key, a_format, args);
            Log(a_key, "%s", "\n");
            va_end(args);
        }

        /**
         * @brief Make a key with the provided values.
         *
         * @param a_format
         * @param ...
         */
        inline std::string TraceTime::MakeKey (const char* const a_format, ...)
        {
            va_list args;
            va_start(args, a_format);
            Write(a_format, args);
            va_end(args);
            return std::string(buffer_);
        }

#ifdef __APPLE__
#pragma mark -
#endif

        /**
         * @brief Reset token rw properties.
         */
        inline void TraceTime::ResetToken (const std::string& a_key, const char* const a_title)
        {
            const auto now = std::chrono::steady_clock::now();

            auto token = tokens_.find(a_key);
            if ( tokens_.end() == token ) {
                return;
            }

            token->second->title_ = a_title;
            if ( 2 != token->second->times_.size() ) {
                token->second->times_.clear();
                for ( uint8_t i = 0 ; i < 2 ; ++i ) {
                    token->second->times_.push_back({now, now});
                }
            } else {
                for ( uint8_t i = 0 ; i < 2 ; ++i ) {
                    token->second->times_[i].start_ = token->second->times_[i].finish_ = now;
                }
            }
            token->second->count_ = 0;
        }

        /**
         * @brief Ensure that the current buffer has at least the required capacity.
         *
         * @param a_capacity The required capacity ( in bytes ).
         */
        inline void TraceTime::EnsureBufferCapacity (const size_t& a_capacity)
        {
            // ... buffer is allocated and has enough capacity? ...
            if ( buffer_capacity_ >= a_capacity ) {
                // ... good to go ...
                return;
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
                throw OutOfMemoryException();
            } else {
                // ... we're good to go ....
                buffer_[0]       = '\0';
                buffer_capacity_ = a_capacity;
            }
        }

        /**
         * @brief Output a log message if the provided token is registered.
         *
         * @param a_key
         * @param a_format
         * @param ...
         */
        inline void TraceTime::Log (const std::string& a_key, const char* const a_format, ...)
        {
            va_list args;
            try {
                va_start(args, a_format);
                Log(a_key, a_format, args);
                va_end(args);
            } catch (std::exception& a_exception) {
                va_end(args);
                throw a_exception;
            }
        }

        /**
         * @brief Output a log message if the provided token is registered.
         *
         * @param a_key
         * @param a_format
         * @param a_args
         */
        inline void TraceTime::Log (const std::string& a_key, const char* const a_format, va_list& a_args)
        {
            // ... write to buffer ...
            const int aux = Write(a_format, a_args);
            // ... ready to output the message ? ...
            if ( aux > 0 && static_cast<size_t>(aux) < buffer_capacity_ ) {
                auto file = tokens_.find(a_key)->second->file_;
                // ... output message ...
                fprintf(tokens_.find(a_key)->second->file_, "%s", buffer_);
                // ... flush ...
                if ( stdout != file && stderr != file ) {
                    fflush(file);
                }
            }
        }

        /**
         * @brief Write a message to a buffer.
         *
         * @param a_format
         * @param ...
         */
        inline int TraceTime::Write (const char* const a_format, va_list& a_args)
        {
            // ... ensure we have a valid buffer ...
            EnsureBufferCapacity(1024);

            int aux = INT_MAX;
            
            va_list args;

            // ... try at least 2 times to construct the output message ...
            for ( uint8_t attempt = 0 ; attempt < 2 ; ++attempt ) {

                buffer_[0] = '\0';
                
                va_copy(args, a_args);
                aux = vsnprintf(buffer_, buffer_capacity_ - 1, a_format, args);
                va_end(args);
                
                if ( aux < 0 ) {
                    // ... an error has occurred ...
                    buffer_[0] = '\0';
                    break;
                } else if ( aux > static_cast<int>(buffer_capacity_) ) {
                    // ... realloc buffer ...
                    EnsureBufferCapacity(static_cast<size_t>(aux + 1));
                } else {
                    // ... all done ...
                    break;
                }
            }

            return aux;
        }

    } // end of namesapce debug

} // end of namespace osal

#endif // NRS_OSAL_DEBUG_TIME_TRACE_H_
