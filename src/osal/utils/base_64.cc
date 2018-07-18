/**
 * @file Base64.cc - Base 64 converter - RFC 3548 implementation ( NOT MIME ).
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

#include "osal/utils/base_64.h"
#include <string.h>
#include <stdlib.h>

const char osal::utils::Base64::kTable64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/**
 * @brief Encode a_buffer to base 64.
 *
 * @param a_buffer
 *                 pointer to an allocated area holding the data to be encoded.
 * @param a_buffer_size
 *                 the a_buffer size.
 * @param o_buffer
 *                 pointer to an allocated area holding the base64 data ( caller must free it ).
 * @param o_buffer_size
 *                 the length of the newly created based 64 buffer.
 *
 * @return         The encode status, one of {@link osal::utils::Base64::Status}.
 *
 */
osal::utils::Base64::Status osal::utils::Base64::Encode (const char* a_buffer, size_t a_buffer_size, char** o_buffer, size_t& o_buffer_size)
{

    const char* in_data       = a_buffer;
	size_t      in_size       = a_buffer_size;
    //
    size_t      out_data_size = 0;
    //
	uint8_t     i_buf[3];
	uint8_t     o_buf[4];

    // nothing encoded ( yet )
    out_data_size = 0;

	// ensure valid params
	if ( a_buffer == NULL || o_buffer == NULL || * o_buffer != NULL ) {
		return osal::utils::Base64::EStatusInvalidParams;
	}

    // calculate the length of a base64-encoded string
	out_data_size = ( in_size / 3) << 2;
	if ( ( in_size % 3 ) > 0 ) {
		out_data_size += 4;
	}

	// terminating null for the encoded String
	out_data_size += 1;

    // allocate memory for the encoded buffer.
	char* out_data = (char*)malloc(sizeof(char)*(out_data_size));
	if ( out_data == NULL ) {
		return osal::utils::Base64::EStatusOutOfMemory;
	}

    // encode
    size_t input_parts = 0;
	while ( in_size > 0 ) {

        // extract 3
		input_parts = 0;
		for ( uint8_t i = 0; i < 3; i++) {
			if ( in_size > 0 ) {
				input_parts++;
				i_buf[i] = *in_data;
				in_data++;
				in_size--;
			} else {
				i_buf[i] = 0;
			}
		}

		// encode to 4
		o_buf[0] = (uint8_t) ((i_buf[0] & 0xFC) >> 2);
		o_buf[1] = (uint8_t) (((i_buf[0] & 0x03) << 4) | ((i_buf[1] & 0xF0) >> 4));
		o_buf[2] = (uint8_t) (((i_buf[1] & 0x0F) << 2) | ((i_buf[2] & 0xC0) >> 6));
		o_buf[3] = (uint8_t) (i_buf[2] & 0x3F);

		switch ( input_parts ) {
			case 1: // only one byte read
			{
				/*
				 * (2) the final quantum of encoding input is exactly 8 bits; here, the
				 * final unit of encoded output will be two characters followed by two
				 * "=" padding characters.
				 */
				out_data[0] = kTable64[o_buf[0]];
				out_data[1] = kTable64[o_buf[1]];
				out_data[2] = '=';
				out_data[3] = '=';
			}
				break;
			case 2: // two bytes read
			{
				/*
				 * (3) the final quantum of encoding input is exactly 16 bits; here, the
				 *     final unit of encoded output will be three characters followed by one
				 *     "=" padding character.
				 */
				out_data[0] = kTable64[o_buf[0]];
				out_data[1] = kTable64[o_buf[1]];
				out_data[2] = kTable64[o_buf[2]];
				out_data[3] = '=';
			}
				break;
			default: // tree bytes read
			{
				/*
				 * (1) the final quantum of encoding input is an integral multiple of 24
				 *	   bits; here, the final unit of encoded output will be an integral
				  *    multiple of 4 characters with no "=" padding.
				 */
				out_data[0] = kTable64[o_buf[0]];
				out_data[1] = kTable64[o_buf[1]];
				out_data[2] = kTable64[o_buf[2]];
				out_data[3] = kTable64[o_buf[3]];
			}
				break;
		}

		out_data += 4;

	}

    // terminate and rewind
	out_data[0]   = 0;
	out_data     -= ( out_data_size - 1);
    //
    (*o_buffer)   = out_data;
    o_buffer_size = out_data_size;
    // done
	return osal::utils::Base64::EStatusOk;
}

/**
 * @brief Encode a_buffer to base 64.
 *
 * @param a_buffer
 * @param o_buffer
 */
osal::utils::Base64::Status osal::utils::Base64::Encode (const std::string& a_buffer, std::string& o_buffer)
{
    char*  out_buffer      = nullptr;
    size_t out_buffer_size = 0;
    const osal::utils::Base64::Status rv = osal::utils::Base64::Encode(a_buffer.c_str(), a_buffer.length(), &out_buffer, out_buffer_size);
    if ( osal::utils::Base64::EStatusOk == rv ) {
        o_buffer = out_buffer;
    } else {
        o_buffer = "";
    }
    osal::utils::Base64::Release(&out_buffer);
    return rv;
}

/**
 * @brief Decode a_buffer from base 64.
 *
 * @param a_buffer
 *                 pointer to an allocated area holding the data to be decoded.
 * @param o_buffer
 *                 pointer to an allocated memory ( the caller must free it ).
 * @param o_buffer_size
 *                 length of the decoded data
 *                 0 if something went wrong
 *
 * @return         The encode status, one of {@link osal::utils::Base64::Status}.
 */
osal::utils::Base64::Status osal::utils::Base64::Decode (const char* a_buffer, uint8_t** o_buffer, size_t& o_buffer_size)
{
	size_t	   	length			= 0;
	size_t  	equals_term		= 0;
	const char*	ptr				= a_buffer;

    // nothing decoded ( yet )
    o_buffer_size = 0;

	// seek for padding
	while ( ( ptr[length] != '=' ) && ptr[length] != 0 ) {
		length++;
	}

    // a maximum of two = padding characters is allowed
	if ( ptr[length] == '=' ) {
		equals_term++;
		if ( ptr[length+equals_term] == '=' ) {
			equals_term++;
		}
	}

    // don't allocate a buffer if the decoded length is 0
	const size_t num_quantums = (length + equals_term) / 4;
	if( num_quantums <= 0 ) {
		return osal::utils::Base64::EStatusDecodeError;
	}

	// set the length of the encoded data
	o_buffer_size = (num_quantums * 3) - equals_term;

	// ensure the buffer is large enough to make room for the last quantum
    // ( which may be partially thrown out ) and the zero terminator
	(*o_buffer) = (uint8_t*)malloc(o_buffer_size + 4 + 1);
	if ( (*o_buffer) == NULL ) {
		return osal::utils::Base64::EStatusOutOfMemory;
	}

	// decode all but the last quantum
	// ( which may not decode to a multiple of 3 bytes ).
	for ( size_t i = 0; i < num_quantums - 1; i++) {
		DecodeQuantum(ptr, (*o_buffer));
		(*o_buffer) += 3;
		ptr        += 4;
	}

	// This final decode may actually read slightly past
	// the end of the buffer if the input string is missing
	// pad bytes.
	uint8_t last_quantum [3];
	DecodeQuantum(ptr, last_quantum);
	for ( size_t i = 0; i < 3 - equals_term; i++ ) {
		(*o_buffer)[0] = last_quantum[i];
		(*o_buffer)++;
	}

	// terminate and rewind
	(*o_buffer)[0]  = 0;
	(*o_buffer)    -= o_buffer_size;

    // done
	return osal::utils::Base64::EStatusOk;
}

/**
 * @brief Decode a base64 buffer.
 *
 * @param a_buffer
 * @param o_buffer
 */
osal::utils::Base64::Status osal::utils::Base64::Decode (const std::string& a_buffer, std::string& o_buffer)
{
    uint8_t* out_buffer      = nullptr;
    size_t   out_buffer_size = 0;
    const osal::utils::Base64::Status rv = osal::utils::Base64::Decode(a_buffer.c_str(), &out_buffer, out_buffer_size);
    if ( osal::utils::Base64::EStatusOk == rv ) {
        o_buffer = std::string(reinterpret_cast<char*>(out_buffer), out_buffer_size);
    } else {
        o_buffer = "";
    }
    osal::utils::Base64::Release(&out_buffer);
    return rv;
}

/**
 * @brief Release the previous allocated buffer by Encode or Decode functions.
 *
 * @param pointer to an allocated memory in the o_buffer
 */
void osal::utils::Base64::Release (char** o_buffer)
{
    if ( (*o_buffer) != NULL ) {
        free((*o_buffer));
        (*o_buffer) = NULL;
    }
}

/**
 * @brief Release the previous allocated buffer by Encode or Decode functions.
 *
 * @param pointer to an allocated memory in the o_buffer
 */
void osal::utils::Base64::Release (uint8_t** o_buffer)
{
    if ( (*o_buffer) != NULL ) {
        free((*o_buffer));
        (*o_buffer) = NULL;
    }
}

/**
 * @brief Decode quantum.
 *
 * @param a_source
 *                 pointer to an allocated area holding the data to be decoded.
 * @param o_destination
 *
 */
osal::utils::Base64::Status osal::utils::Base64::DecodeQuantum (const char* a_source, uint8_t* o_destination)
{
	//
	size_t x = 0;
    //
	for ( size_t i = 0; i < 4; i++) {
        const char* found = strchr(kTable64, a_source[i]);
		if ( found != NULL ) {
			x = (x << 6) + (unsigned int)(found - kTable64);
		} else if ( a_source[i] == '=' ) {
			x = (x << 6);
		}
	}
    //
	o_destination[2] = (uint8_t)(x & 255);
	x >>= 8;
	o_destination[1] = (uint8_t)(x & 255);
	x >>= 8;
	o_destination[0] = (uint8_t)(x & 255);

    // done
    return osal::utils::Base64::EStatusOk;
}

//endof $Id: $
