/**
 * @file utf8_string.cc -
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

#include "osal/utf8_string.h"

#include "osal/exception.h"
#include "osal/utils/utf8_utils.h"

#include <sstream>    // std::istringstream
#include <functional> // std::function
#include <algorithm>  // std::find_if
#include <regex>      // std::regex

#ifdef __APPLE__
#pragma mark - osal::UTF8StringIteraror
#endif

/**
 * @brief C string constructor.
 */
osal::UTF8StringIteraror::UTF8StringIteraror (const char* a_string)
    : start_ptr_(a_string)
{
    current_ptr_           = start_ptr_;
    current_char_          = 0;
}

/**
 * @brief Standard C++ string.
 */
osal::UTF8StringIteraror::UTF8StringIteraror (const std::string& a_string)
    : start_ptr_(a_string.c_str())
{
    current_ptr_           = start_ptr_;
    current_char_          = 0;
}

/**
 * @brief Destructor
 */
osal::UTF8StringIteraror::~UTF8StringIteraror ()
{
    /* empty */
}

/**
 * @brief Count the number of utf8 chars in a string.
 *
 * @param a_string
 *
 * @return The number of UTF8 chars.
 */
size_t osal::UTF8StringIteraror::CharsCount (const std::string& a_string)
{
    osal::UTF8StringIteraror it = osal::UTF8StringIteraror(a_string);

    size_t count = 0;
    while ( 0 != it.Next() ) {
        count++;
    }
    return count;
}

/**
 * @brief Count the number of UTF8 chars and bytes in a string.
 *
 * @param a_string
 *
 * @return The number of UTF8 chars and bytes in a string.
 */
std::tuple<size_t, size_t> osal::UTF8StringIteraror::Count (const std::string& a_string)
{
    return std::make_tuple(a_string.length() > 0 ? osal::UTF8StringIteraror::CharsCount(a_string) : 0, a_string.length());
}

#ifdef __APPLE__
#pragma mark - osal::UTF8StringHelper
#endif

/**
 * @brief A helper function to check if a string starts with another string.
 *
 * @param a_first  Initial string.
 * @param a_second Prefix string
 */
bool osal::UTF8StringHelper::StartsWith (const std::string& a_first, const std::string& a_second)
{
    osal::UTF8StringIteraror first_string  = osal::UTF8StringIteraror(a_first);
    osal::UTF8StringIteraror second_string = osal::UTF8StringIteraror(a_second);

    size_t number_of_chars = std::min(first_string.Remaining(), second_string.Remaining());
   	if ( number_of_chars == 0 ) {
        return true;
   	}

    if ( a_first.length() < a_second.length() ) {
        return false;
    }

    do {
        uint16_t c1 = first_string.Next();
        if ( c1 == 0 ) {
            break;
        }

        // lower case
        if ( c1 < sizeof(osal::utils::UTF8Utils::k_utf8_collation_table_lower_)/sizeof(uint16_t) ) {
            c1 = osal::utils::UTF8Utils::k_utf8_collation_table_lower_[c1];
        }

        uint16_t c2 = second_string.Next();
        if ( c2 < sizeof(osal::utils::UTF8Utils::k_utf8_collation_table_lower_)/sizeof(uint16_t) ) {
            c2 = osal::utils::UTF8Utils::k_utf8_collation_table_lower_[c2];
        }

        if ( c2 == 0 ) {
            return false;
        } else if ( c1 != c2 ) {
            // normalized
            c1 = osal::utils::UTF8Utils::k_utf8_collation_table_[c1];
            c2 = osal::utils::UTF8Utils::k_utf8_collation_table_[c2];
            if ( c1 != c2 ) {
                return false;
            }
        }
    } while ( --number_of_chars != 0 );

    return true;
}

/**
 * @brief A helper function to check if a string contains another string.
 *
 * @param a_first  Initial string.
 * @param a_second The other string
 */
bool osal::UTF8StringHelper::Contains (const std::string& a_first, const std::string& a_second)
{
    return nullptr != strstr(a_first.c_str(), a_second.c_str());
}

/**
 * @brief A helper a text should be rejected for a specific filter.
 *
 * @param a_string String to be tested.
 * @param a_filter Filter to be applied.
 * @param o_words  String words.
 */
bool osal::UTF8StringHelper::FilterOut (const std::string& a_string, const std::string& a_filter, std::vector<std::string>* o_words)
{
    const char* string_c_str   = a_string.c_str();
    const char* filter_c_str = a_filter.c_str();

    if ( nullptr != o_words ) {
        o_words->clear();
    } else if ( 0 == a_string.length() && 0 == a_filter.length() ) {
        return false;
    } else  if ( filter_c_str[0] == 0 ) {
        return false;
    } else if ( string_c_str[0] == 0 ) {
        return true;
    }

    std::vector<std::string> text_words;
    std::vector<std::string> filter_words;

    SplitWords(a_string, text_words);
    SplitWords(a_filter, filter_words);

    if ( filter_words.size() <= text_words.size() ) {
        for ( auto text_words_iterator = text_words.begin(); text_words_iterator != text_words.end(); ++text_words_iterator ) {
            for ( auto filter_words_iterator = filter_words.begin(); filter_words_iterator != filter_words.end(); ++filter_words_iterator ) {
                if ( true == osal::UTF8StringHelper::StartsWith((*text_words_iterator), (*filter_words_iterator)) ) {
                    if ( nullptr != o_words ) {
                        o_words->push_back((*filter_words_iterator));
                    }
                    filter_words.erase(filter_words_iterator);
                    break;
                }
            }
        }
        return filter_words.size() != 0;
    }

    return true;
}

/**
 * @brief A helper a split a string in to words.
 *
 * @param a_string String to be splitted.
 * @param o_words  String words.
 */
void osal::UTF8StringHelper::SplitWords (const std::string& a_string, std::vector<std::string>& o_words)
{
    o_words.clear();

    std::istringstream stream(a_string);
    std::string        word;

    while ( std::getline(stream, word, ' ') ) {
        if ( word.length() ) {
            o_words.push_back(word);
        }
    }
}

/**
 * @brief A helper a replace a string.
 *
 * @param a_string The string to be processed.
 * @param a_from   The string to replace.
 * @param a_to     The new string.
 * @param o_string The final result, with all matches replaced
 */
void osal::UTF8StringHelper::Replace (const std::string& a_string, const std::string& a_from, const std::string& a_to, std::string& o_string)
{
    size_t pos = 0;

    o_string = a_string;

    while ( std::string::npos != ( pos = o_string.find(a_from, pos) ) ) {
        o_string.replace(pos, a_from.length(), a_to);
        pos += o_string.length();
    }
}

/**
 * @brief Search and replace all occurrences of a string.
 *
 * @param a_source
 * @param a_search
 * @param a_replace
 *
 * @return
 */
std::string osal::UTF8StringHelper::ReplaceAll (const std::string& a_source, const std::string& a_search, const std::string& a_replace)
{
    std::string source = a_source;
    for ( std::string::size_type i = 0 ; ( i = source.find(a_search, i) ) != std::string::npos ; ) {
        source.replace(i, a_search.length(), a_replace);
        i += a_replace.length();
    }
    return source;
}

/**
 * @brief A helper function to remove white spaces from the right and left of a string.
 *
 * @param a_string String to be processed
 *
 * @return The final result, with the white spaces removed from left and right.
 */
std::string& osal::UTF8StringHelper::Trim (std::string& a_string)
{
    a_string.erase(a_string.begin(),
                   std::find_if(a_string.begin(), a_string.end(),
				[](char& ch)->bool {
				  return !isspace(ch);
				}
		   )
    );
    a_string.erase(std::find_if(a_string.rbegin(), a_string.rend(),
				[](char& ch)->bool {
				  return !isspace(ch);
				}
       	           ).base(),
                   a_string.end()
    );
    return a_string;
}

/**
 * @brief A helper function to 'collate' a string.
 *
 * @param a_string String to be processed
 *
 * @return The final result.
 */
std::string osal::UTF8StringHelper::Collate (const char* const a_string)
{
    osal::UTF8StringIteraror iterator = osal::UTF8StringIteraror(a_string);
    
    size_t number_of_chars = iterator.Remaining();
   	if ( number_of_chars == 0 ) {
        return std::string(a_string);
   	}
    
    std::stringstream ss;
    
    uint8_t bytes[4];
    
    do {
        uint16_t utf_16_char = iterator.Next();
        if ( utf_16_char == 0 ) {
            break;
        }
        
        if ( utf_16_char < sizeof(osal::utils::UTF8Utils::k_utf8_collation_table_)/sizeof(uint16_t) ) {
            utf_16_char = osal::utils::UTF8Utils::k_utf8_collation_table_[utf_16_char];
        }
        
        if ( utf_16_char <= 0x7F ) {           /* 0XXX XXXX one byte     */
            bytes[0] = (uint8_t) utf_16_char;
            bytes[1] = 0;
        } else if ( utf_16_char <= 0x7FF ) {   /* 110X XXXX two bytes   */
            bytes[0] = (uint8_t) ( 0xC0 | (utf_16_char >> 6)  );
            bytes[1] = (uint8_t) ( 0x80 | (utf_16_char & 0x3F));
            bytes[2] = 0;
        } else if ( utf_16_char <= 0xFFFF ) {  /* 1110 XXXX three bytes */
            bytes[0] = (uint8_t) (0xE0 | ( utf_16_char >> 12        ));
            bytes[1] = (uint8_t) (0x80 | ((utf_16_char >> 6) & 0x3F ));
            bytes[2] = (uint8_t) (0x80 | ( utf_16_char       & 0x3F ));
            bytes[3] = 0;
        } else {
            bytes[0] = 0;
        }
        
        ss << bytes;
        
    } while ( --number_of_chars != 0 );
    
    return ss.str();
}

/**
 * @brief A helper function to remove white spaces from the right and left of a string.
 *
 * @param a_string String to be processed
 *
 * @return The final result, with the white spaces removed from left and right.
 */
std::string osal::UTF8StringHelper::Trim (const char* const a_string)
{
    std::string str = a_string;
    return osal::UTF8StringHelper::Trim(str);
}

/**
 * @brief A helper to encode an URI.
 *
 * @param a_string The URI string.
 * @param o_string The encoded URI string.
 *
 * @throw An \link osal::Exception \link on error.
 */
void osal::UTF8StringHelper::URIEncode (const std::string& a_string, std::string& o_string)
{
    static const char kDec2Hex[16 + 1] = "0123456789ABCDEF";

    const size_t         limit  = a_string.length();
    unsigned char* const buffer = new unsigned char[limit * 3];
    if ( nullptr == buffer ) {
        throw OSAL_EXCEPTION_NA("Out of memory error!");
    }

    unsigned char*       buffer_end_ptr = buffer;
    const unsigned char* current_ptr    = (const unsigned char *)a_string.c_str();

    const unsigned char* const end_ptr = current_ptr + limit;

    for  ( ; current_ptr < end_ptr; ++current_ptr ) {
        if ( ( *current_ptr >= 'a' && *current_ptr <= 'z' ) || ( *current_ptr >= 'A' && *current_ptr <='Z' ) || ( *current_ptr >= '0' && *current_ptr <= '9' ) ) {
            // just copy
            *buffer_end_ptr++ = *current_ptr;
        } else {
            // escape this char
            *buffer_end_ptr++ = '%';
            *buffer_end_ptr++ = static_cast<unsigned char>(kDec2Hex[*current_ptr >> 4]);
            *buffer_end_ptr++ = static_cast<unsigned char>(kDec2Hex[*current_ptr & 0x0F]);
        }
    }

    o_string = std::string((const char*)buffer, (const char*)buffer_end_ptr);

    delete [] buffer;
}

/**
 * @brief A helper to encode an URI params.
 *
 * @param a_uri The URI string.
 *
 * @return The encoded URI.
 *
 * @throw An \link osal::Exception \link on error.
 */
std::string osal::UTF8StringHelper::URIEncode (const std::string& a_uri)
{
    // ... if uri is empty ...
    if ( 0 == a_uri.length() ) {
        // ... no URI to encode  ...
        return "";
    }
    const char* const uri_start_ptr = a_uri.c_str();
    // ... pick query start ...
    const char* const query_start_ptr = strchr(uri_start_ptr, '?');
    if ( nullptr == query_start_ptr ) {
        // ... no query, no params to encode ...
        return a_uri;
    }
    
    std::string encoded_uri;
    
    // ... pick new url start ...
    if ( query_start_ptr != uri_start_ptr ) {
        // ... from the beginning until the start of the query ...
        encoded_uri = std::string(uri_start_ptr, static_cast<size_t>(query_start_ptr - uri_start_ptr));
    } else {
        // ... uri starts with query separator ...
        encoded_uri = "";
    }
    
    // ... set uri end and pick set query start ...
    const char* const uri_end_ptr = uri_start_ptr + a_uri.length();
    const char*       query_ptr   = query_start_ptr;
    
    std::string param_value;
    
    // ... split params and encode it's value only ....
    const std::regex params_expr("(\\?|\\&)([^=]+)\\=([^&]+)?", std::regex_constants::ECMAScript | std::regex_constants::icase);
    auto tmp_begin = std::sregex_iterator(a_uri.begin(), a_uri.end(), params_expr);
    auto tmp_end   = std::sregex_iterator();
    // ... for each match ...
    for ( std::sregex_iterator tmp_it = tmp_begin; tmp_it != tmp_end; ++tmp_it ) {
        // ... pick match ...
        const std::smatch match = *tmp_it;
        // ... if failed, no point in continue ...
        if ( 0 == match.size() || 4 != match.size() ) {
            break;
        }
        // ... copy <name>=ENCODE(<value>) ...
        encoded_uri   += match[1];
        encoded_uri   += match[2];
        encoded_uri   += '=';
        // ... param is not empty?
        if ( match[3].length() > 0 ) {
            URIEncode(match[3], param_value);
            encoded_uri += param_value;
        }
        // ... advance query pointer ...
        query_ptr += match[0].str().length();
    }
    // ... append remaining uri ( a.k.a. fragment ) ....
    if ( uri_end_ptr != query_ptr ) {
        encoded_uri += std::string(query_ptr, static_cast<size_t>(uri_end_ptr - query_ptr));
    }
    // ... we're done ..
    return encoded_uri;
}

/**
 * @brief A helper to decode an URI.
 *
 * @param a_string The encoded URI string.
 * @param o_string The decoded URI string.
 *
 * @throw An \link osal::Exception \link on error.
 */
void osal::UTF8StringHelper::URIDecode (const std::string & a_string, std::string& o_string)
{
    //
    // RFC1630:
    //
    // "Sequences which start with a percent sign but are not followed by two hexadecimal characters (0-9, A-F) are reserved for future extension"
    //

    const std::function<char(const char a_char)> hex2dec = [] (const char a_char) {
        return (char)( a_char - ( ( a_char <= '9' ) ? '0' : ( a_char >= 'a' ) ? 'a'-10 : 'A'-10 ) );
    };

    const size_t         limit   = a_string.length();
    unsigned char* const buffer = new unsigned char[limit];
    if ( nullptr == buffer ) {
        throw OSAL_EXCEPTION_NA("Out of memory error!");
    }
    unsigned char* buffer_end_ptr = buffer;

    const unsigned char*       source_ptr             = (const unsigned char *)a_string.c_str();
    const unsigned char* const end_ptr                 = source_ptr + limit;
    const unsigned char* const last_decoded_symbol_ptr = end_ptr - 2;

    for ( ; source_ptr < last_decoded_symbol_ptr ; ) {
        if ( '%' == *source_ptr ) {
            char dec1, dec2;
            if ( -1 != ( dec1 = hex2dec(char(*(source_ptr + 1))) ) && -1 != ( dec2 = hex2dec(char(*(source_ptr + 2))) ) ) {
                // unescape this char
                *buffer_end_ptr++ = (unsigned char) ( dec1 << 4 ) + dec2;
                source_ptr += 3;
                continue;
            }
        }
        // just copy
        *buffer_end_ptr++ = *source_ptr++;
    }

    // the last 2 chars
    while ( source_ptr < end_ptr ) {
        *buffer_end_ptr++ = *source_ptr++;
    }

    o_string = std::string((const char*)buffer, (const char*)buffer_end_ptr);

    delete [] buffer;
}

/**
 * @brief Count the number of UTF8 chars and bytes in a string.
 *
 * @param a_string
 *
 * @return The number of UTF8 chars and bytes in a string.
 */
std::tuple<size_t, size_t> osal::UTF8StringHelper::CountForJson (const std::string& a_string)
{
    if ( a_string.length() == 0 ) {
        return std::make_tuple(0,0);
    } else {
        osal::UTF8StringIteraror it = osal::UTF8StringIteraror(a_string);
        uint16_t utf_16_char;
        
        size_t count = 0;
        while ( (utf_16_char = it.Next()) != 0 ) {
            switch (utf_16_char) {
                case '"':
                case '\\':
                case '/':
                case '\b':
                case '\f':
                case '\n':
                case '\r':
                case '\t':
                    count += 2;
                    break;
                default:
                    count++;
                    break;
            }
        }
        return std::make_tuple(count, a_string.length());
    }
}

/**
 * @brief Encode string as JSON, escapes the special chars
 *
 * @param a_string input string not changed
 * @param o_out output buffer must have at least twice the length of the input stream plus one
 */
void osal::UTF8StringHelper::JSONEncode (const std::string& a_string, uint8_t* o_out)
{
    osal::UTF8StringIteraror it = osal::UTF8StringIteraror(a_string);
    uint16_t utf_16_char;

    while ( (utf_16_char = it.Next()) != 0 ) {
        switch (utf_16_char) {
            case '"':
                *(o_out++) = (uint8_t) '\\';
                *(o_out++) = (uint8_t) '"';
                break;
            case '\\':
                *(o_out++) = (uint8_t) '\\';
                *(o_out++) = (uint8_t) '\\';
                break;
            case '/':
                *(o_out++) = (uint8_t) '\\';
                *(o_out++) = (uint8_t) '/';
                break;
            case '\b':
                *(o_out++) = (uint8_t) '\\';
                *(o_out++) = (uint8_t) 'b';
                break;
            case '\f':
                *(o_out++) = (uint8_t) '\\';
                *(o_out++) = (uint8_t) 'f';
                break;
            case '\n':
                *(o_out++) = (uint8_t) '\\';
                *(o_out++) = (uint8_t) 'n';
                break;
            case '\r':
                *(o_out++) = (uint8_t) '\\';
                *(o_out++) = (uint8_t) 'r';
                break;
            case '\t':
                *(o_out++) = (uint8_t) '\\';
                *(o_out++) = (uint8_t) 't';
                break;

            default:

                if (utf_16_char <= 0x7F)  {          /* 0XXX XXXX one byte     */

                    *(o_out++) = (uint8_t) utf_16_char;

                } else if (utf_16_char <= 0x7FF) {   /* 110X XXXX  two bytes   */

                    *(o_out++) = (uint8_t) ( 0xC0 | (utf_16_char >> 6)  );
                    *(o_out++) = (uint8_t) ( 0x80 | (utf_16_char & 0x3F));

                } else if (utf_16_char <= 0xFFFF)  { /* 1110 XXXX  three bytes */

                    *(o_out++) = (uint8_t) (0xE0 | ( utf_16_char >> 12        ));
                    *(o_out++) = (uint8_t) (0x80 | ((utf_16_char >> 6) & 0x3F ));
                    *(o_out++) = (uint8_t) (0x80 | ( utf_16_char       & 0x3F ));

                }
                break;
        }
    }
    *(o_out) = 0;
}

/**
 * @brief Encode string as JSON, escapes the special chars
 *
 * @param a_string input string not changed
 *
 * @return
 */
std::string osal::UTF8StringHelper::JSONEncode (const std::string& a_string)
{
    uint8_t* out = new uint8_t[( a_string.length() * 2 ) + 1];
    if ( nullptr == out ) {
        throw OSAL_EXCEPTION_NA("Out of memory error!");
    }
    osal::UTF8StringHelper::JSONEncode(a_string, out);
    const auto rv = std::string((char*)out);
    delete [] out;
    return rv;
}
