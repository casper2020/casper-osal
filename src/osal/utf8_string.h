#pragma once
/**
 * @file utf8_string.h -
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

#ifndef NRS_OSAL_UTF8_STRING_H_
#define NRS_OSAL_UTF8_STRING_H_

#include "osal/osalite.h"

#include <string> // std::string
#include <vector> // std::vector
#include <map>    // std::map
#include <tuple>  // std::tuple

namespace osal
{

    /**
     * @brief A UTF8 String iterator
     */
    class UTF8StringIteraror
    {

    private: // Data

        const char*         start_ptr_;
        const char*         current_ptr_;
        uint16_t            current_char_;

    public: // Constructor(s) / Destructor

        UTF8StringIteraror (const char* a_string);
        UTF8StringIteraror (const std::string& a_string);
        virtual ~UTF8StringIteraror ();

    public: // Inline Method(s) / Function(s)

        inline uint16_t Current   () const;
        inline size_t   Remaining () const;
        inline uint16_t Next      ();

    public: // Overloaded Operator(s)

        inline UTF8StringIteraror& operator ++ ();
        inline UTF8StringIteraror& operator += (int);
        inline UTF8StringIteraror& operator -- ();
        inline UTF8StringIteraror& operator -= (int);

        inline UTF8StringIteraror& operator  = (const char*);
        inline UTF8StringIteraror& operator  = (const UTF8StringIteraror&);

        inline bool operator == (const char*);
        inline bool operator == (const UTF8StringIteraror&);

    public: // Static Method(s) / Function(s)

        static size_t                       CharsCount   (const std::string& a_string);
        static std::tuple<size_t, size_t>   Count        (const std::string& a_string);

    }; // end of class UTF8Iteraror

    /**
     * @brief Return the current UTF8 char.
     */
    inline uint16_t UTF8StringIteraror::Current() const
    {
        return current_char_;
    }

    /**
     * @return The number of remaining UTF8 chars.
     */
    inline size_t UTF8StringIteraror::Remaining () const
    {
        const char* start_ptr    = start_ptr_;
        const char* current_ptr  = current_ptr_;
        size_t      count        = 0;
        uint16_t    current_char = 0;

        if ( '\0' == (*start_ptr) ) {
            return 0;
        }

        do {
            current_char = static_cast<uint16_t>(*(current_ptr++));
            if ( current_char >= 0x80 ) {
                if ( (current_char & 0xE0) == 0xC0 ) {
                    current_char &= 0x1F;
                    current_char <<= 6;
                    current_char |= *(current_ptr++) & 0x3F;
                } else {
                    current_char &= 0x0F;
                    current_char <<= 6;
                    current_char |= *(current_ptr++) & 0x3F;
                    current_char <<= 6;
                    current_char |= *(current_ptr++) & 0x3F;
                }
            }
            count++;
        } while ( 0 != current_char );

        return count - 1; // -1 ~> \0a
    }

    /**
     * @brief Overloaded '++' operator.
     */
    inline UTF8StringIteraror& UTF8StringIteraror::operator ++ ()
    {
        throw OSAL_EXCEPTION_NA("Unsupported operator++!");
    }

    /**
     * @brief Overloaded '+=' operator.
     */
    inline UTF8StringIteraror& UTF8StringIteraror::operator += (int)
    {
        throw OSAL_EXCEPTION_NA("Unsupported operator+=!");
    }

    /**
     * @brief Overloaded '--' operator.
     */
    inline UTF8StringIteraror& UTF8StringIteraror::operator -- ()
    {
        throw OSAL_EXCEPTION_NA("Unsupported operator--!");
    }

    /**
     * @brief Overloaded '+=' operator.
     */
    inline UTF8StringIteraror& UTF8StringIteraror::operator -= (int)
    {
        throw OSAL_EXCEPTION_NA("Unsupported operator-=!");
    }

    /**
     * @brief Overloaded '=' operator.
     */
    inline UTF8StringIteraror& UTF8StringIteraror::operator = (const char* a_string)
    {
        start_ptr_    = a_string;
        current_ptr_  = start_ptr_;
        current_char_ = 0;
        return *this;
    }

    /**
     * @brief Overloaded '=' operator.
     */
    inline UTF8StringIteraror& UTF8StringIteraror::operator = (const UTF8StringIteraror& a_iterator)
    {
        start_ptr_    = a_iterator.start_ptr_;
        current_ptr_  = a_iterator.current_ptr_;
        current_char_ = a_iterator.current_char_;
        return *this;
    }

    /**
     * @brief Overloaded '==' operator.
     */
    inline bool UTF8StringIteraror::operator == (const char* a_string)
    {
        if ( nullptr == a_string || nullptr == start_ptr_ ) { // both null
            return start_ptr_ == a_string;
        }
        // same content?
        return 0 == strcmp(start_ptr_, a_string);
    }

    /**
     * @brief Overloaded '==' operator.
     */
    inline bool UTF8StringIteraror::operator == (const UTF8StringIteraror& a_iterator)
    {
        // same content?
        return ( a_iterator.start_ptr_ == start_ptr_ && a_iterator.current_ptr_ == current_ptr_ );
    }

    /**
     * @brief Advance to next char.
     *
     * @return The current char value.
     */
    inline uint16_t UTF8StringIteraror::Next ()
    {
        current_char_ = static_cast<uint16_t>(*(current_ptr_++));
        if ( current_char_ >= 0x80 ) {
            if ( (current_char_ & 0xE0) == 0xC0 ) {
                current_char_ &= 0x1F;
                current_char_ <<= 6;
                current_char_ |= *(current_ptr_++) & 0x3F;
            } else {
                current_char_ &= 0x0F;
                current_char_ <<= 6;
                current_char_ |= *(current_ptr_++) & 0x3F;
                current_char_ <<= 6;
                current_char_ |= *(current_ptr_++) & 0x3F;
            }
        }
        return current_char_;
    }

#ifdef __APPLE__
#pragma mark - UTF8StringHelper
#endif

    /**
     * @brief A UTF8 string helper.
     */
    class UTF8StringHelper
    {

    public: // Static Method(s) / Function(s)

        /**
         * @brief A helper function to check if a string starts with another string.
         *
         * @param a_first  Initial string.
         * @param a_second Prefix string
         */
        static bool StartsWith (const std::string& a_first, const std::string& a_second);

        /**
         * @brief A helper function to check if a string contains another string.
         *
         * @param a_first  Initial string.
         * @param a_second The other string
         */
        static bool Contains (const std::string& a_first, const std::string& a_second);

        /**
         * @brief A helper a text should be rejected for a specific filter.
         *
         * @param a_string String to be tested.
         * @param a_filter Filter to be applied.
         * @param o_words  String words.
         */
        static bool FilterOut  (const std::string& a_string, const std::string& a_filter, std::vector<std::string>* o_words);

        /**
         * @brief A helper a split a string in to words.
         *
         * @param a_string String to be splitted.
         * @param o_words  String words.
         */
        static void SplitWords (const std::string& a_string, std::vector<std::string>& o_words);

        /**
         * @brief A helper a replace a string.
         *
         * @param a_string The string to be processed.
         * @param a_from   The string to replace.
         * @param a_to     The new string.
         * @param o_string The final result, with all matches replaced
         */
        static void Replace (const std::string& a_string, const std::string& a_from, const std::string & a_to, std::string& o_string);

        /**
         * @brief Search and replace all occurrences of a string.
         *
         * @param a_source
         * @param a_search
         * @param a_replace
         *
         * @return
         */
        static std::string ReplaceAll (const std::string& a_source, const std::string& a_search, const std::string& a_replace);

        /**
         * @brief A helper function to remove white spaces from the right and left of a string.
         *
         * @param a_string String to be processed
         *
         * @return The final result, with the white spaces removed from left and right.
         */
        static std::string& Trim (std::string& a_string);

        /**
         * @brief A helper function to remove white spaces from the right and left of a string.
         *
         * @param a_string String to be processed
         *
         * @return The final result, with the white spaces removed from left and right.
         */
        static std::string Trim (const char* const a_string);

        /**
         * @brief A helper function to 'collate' a string.
         *
         * @param a_string String to be processed
         *
         * @return The final result.
         */
        static std::string Collate (const char* const a_string);

        /**
         * @brief A helper to encode an URI.
         *
         * @param a_string The URI string.
         * @param o_string The encoded URI string.
         *
         * @throw An \link osal::Exception \link on error.
         */
        static void URIEncode (const std::string& a_string, std::string& o_string);
        
        /**
         * @brief A helper to encode an URI params.
         *
         * @param a_uri The URI string.
         *
         * @return The encoded URI.
         *
         * @throw An \link osal::Exception \link on error.
         */
        static std::string URIEncode (const std::string& a_uri);

        /**
         * @brief A helper to decode an URI.
         *
         * @param a_string The encoded URI string.
         * @param o_string The decoded URI string.
         *
         * @throw An \link osal::Exception \link on error.
         */
        static void URIDecode (const std::string& a_string, std::string& o_string);

        /**
         * @brief Count the number of UTF8 chars and bytes in a string.
         *
         * @param a_string
         *
         * @return The number of UTF8 chars and bytes in a string.
         */
        static std::tuple<size_t, size_t> CountForJson (const std::string& a_string);


        static void JSONEncode (const std::string& a_string, uint8_t* o_out);
        
        static std::string JSONEncode (const std::string& a_string);

    }; // end of class UTF8StringHelper

} // end of namespace osal

#endif // NRS_OSAL_UTF8_STRING_H_
