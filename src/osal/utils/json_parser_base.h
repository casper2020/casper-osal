#pragma once
/**
 * @file json_parseable.h
 *
 * Base class for objects that are parsed from JSON
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

#ifndef NRS_OSAL_UTILS_JSON_PARSERABLE_H
#define NRS_OSAL_UTILS_JSON_PARSERABLE_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <stdlib.h> // realloc

#include "unicode/normalizer2.h" // U_ICU_NAMESPACE::Normalizer2

namespace osal {

    namespace utils {

        class JsonParserBase
        {
        public: // static data

            static const uint8_t kHexTable[256];
            static const uint8_t k_max_fractional_digits_;

        protected:

            /*
             * Data model for number parser
             */
            bool     negative_;
            uint64_t integer_;
            uint64_t fractional_;
            uint8_t  fractional_digits_cnt_;            
            uint64_t divider_;
            uint32_t div_log10_;
            bool     negative_exponent_;
            uint64_t exponent_;
            int64_t  int_value_;
            double   double_value_;
            int      stack_size_;
            int*     stack_;
            int      top_;

            /*
             * Data model for boolean parser
             */
            bool     boolean_value_;

            /*
             * Data model for string parser
             */
            char*    string_start_;
            size_t   string_length_;
            bool     string_has_escapes_;

            /*
             * Data model for incremental parser
             */
            uint32_t write_buffer_capacity_;
            int8_t*  write_buffer_;
            int8_t*  write_start_;
            int8_t*  write_pointer_;
            int8_t*  write_limit_;
            bool     write_failed_;

            uint32_t code_point_;
            uint32_t code_point_shift_;
            bool     parse_completed_;

            /*
             * Ragel state variables
             */
            int      cs_;
            int      error_col_;

        protected:

            bool        success_;
            std::string last_error_;

        protected: // ICU

            const U_ICU_NAMESPACE::Normalizer2* icu_normalizer_ptr_;     //!< UTF8 string normalizer.
            UErrorCode                          icu_error_code_;         //!<
            std::string                         icu_normalized_string_;  //!<

        protected:

            char*  CloneString (char* a_string, int a_length) const;
            double Pow10       (int n);

        public:

            JsonParserBase (uint32_t a_capacity = 16 * 1024 /* 16KB */);
            virtual ~JsonParserBase ();

            // Single shot parser interface
            virtual bool   Parse (const char* a_json, size_t a_lenght);

            // Incremental parser interface
            virtual bool   InitParse           ();
            virtual bool   ParseSlice          (const char* a_json, size_t size_t);
            virtual bool   FinishParse         ();

            // Utility
            double GetDouble ();

            static void CopyBytes (int8_t* a_dst, int8_t* a_src, int32_t a_size);

            bool Success () const;
            const std::string& LastError () const;
            const std::string& NormalizeString (const char* a_string);
            
            bool WriteBufferHasEnoughSpace (size_t a_amount);
            bool ReallocWriterBuffer (size_t a_min_amount = 0);

        };

        inline double JsonParserBase::GetDouble ()
        {
            if ( divider_ == 1 && exponent_ == 0 ) {
                return (double) int_value_;
            } else {
                return double_value_;
            }
        }

        inline void JsonParserBase::CopyBytes (int8_t* a_dst, int8_t* a_src, int32_t a_size)
        {
            while (a_size--) {
                *(a_dst++) = *(a_src++);
            }
        }

        inline bool JsonParserBase::Success () const
        {
            return success_;
        }
    
        inline const std::string& JsonParserBase::LastError () const
        {
            return last_error_;
        }

        inline const std::string& JsonParserBase::NormalizeString (const char* a_string)
        {
            if ( nullptr == icu_normalizer_ptr_ ) {
                icu_normalized_string_ = a_string;
                return icu_normalized_string_;
            }

            U_ICU_NAMESPACE::UnicodeString unicode_string = U_ICU_NAMESPACE::UnicodeString::fromUTF8(a_string);

            icu_normalized_string_ = "";
            icu_normalizer_ptr_->normalize(unicode_string, icu_error_code_);
            if ( UErrorCode::U_ZERO_ERROR == icu_error_code_ ) {
                unicode_string.toUTF8String(icu_normalized_string_);
            } else {
                icu_normalized_string_ = a_string;
                icu_error_code_        = UErrorCode::U_ZERO_ERROR;
            }

            return icu_normalized_string_;
        }
    
        inline bool JsonParserBase::WriteBufferHasEnoughSpace (size_t a_amount)
        {
            return ( ( (write_pointer_ - write_start_) + a_amount )  <= write_buffer_capacity_ );
        }
    
        inline bool JsonParserBase::ReallocWriterBuffer (size_t a_min_amount)
        {
            const size_t current_offset  = write_pointer_ - write_start_;
            write_buffer_capacity_ *= 2;
            if ( 0 != a_min_amount && write_buffer_capacity_ < static_cast<uint32_t>(a_min_amount) ) {
                write_buffer_capacity_ += ( static_cast<uint32_t>(a_min_amount) - write_buffer_capacity_ );
            }
            write_buffer_ = (int8_t*)realloc(write_buffer_, sizeof(int8_t) * static_cast<size_t>(write_buffer_capacity_));
            if ( NULL != write_buffer_ ) {
                write_start_           = write_buffer_;
                write_pointer_         = write_buffer_ + current_offset;
                write_limit_           = write_pointer_ + write_buffer_capacity_;
            } else {
                write_buffer_capacity_ = 0;
                write_start_           = nullptr;
                write_pointer_         = nullptr;
                write_limit_           = nullptr;
            }
            return ( nullptr != write_buffer_ );
        }

    } // endof namespace utils
} // endof namespace osal

#endif // NRS_OSAL_UTILS_JSON_PARSERABLE_H
