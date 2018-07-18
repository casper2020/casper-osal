#pragma once
/**
 * @file exception.h declaration of an exception class that carries a user message and
 *       the location of the fault origin
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
#ifndef NRS_OSAL_EXCEPTION_H
#define NRS_OSAL_EXCEPTION_H

#include <stdexcept>
#include <exception>
#include <string>

#define OSAL_EXCEPTION(FORMAT,...)       osal::Exception(__FILE__, __LINE__, false, FORMAT,  __VA_ARGS__)
#define OSAL_NULL_EXCEPTION(FORMAT,...)  osal::Exception(__FILE__, __LINE__, true,  FORMAT,  __VA_ARGS__)
#define OSAL_EXCEPTION_NA(FORMAT)        osal::Exception(__FILE__, __LINE__, false, FORMAT)
#define OSAL_NULL_EXCEPTION_NA(FORMAT)   osal::Exception(__FILE__, __LINE__, false, FORMAT)

#define OSAL_EXCEPTION_WITH_CODE(CODE, FORMAT,...) osal::Exception(CODE, __FILE__, __LINE__, false, FORMAT,  __VA_ARGS__)

namespace osal
{
    /**
     * @brief An exception class that carries a user message and the location
     *        of the fault origin
     */
    class Exception : std::exception
    {
    protected: // Child Elements

        const char* file_;
        int         line_;
        char        message_[1024];
        bool        is_null_;
        
        std::string code_;

    protected: // Methods

        char* StealMessage ();

    public: // Methods

        Exception (const char* a_code, const char* a_file, int a_line, bool a_is_null, const char* a_format, ...) throw();
        Exception (const char* a_file, int a_line, bool a_is_null, const char* a_format, ...) throw();
        Exception (bool a_is_null, const char* a_format, ...)    throw();
        Exception (const Exception& a_exception) throw();
        virtual ~Exception () throw();


        bool IsNull  ();
        void SetNull (bool a_null);

        const char* Message () const;
        const char* File    () const;
        int         Line    () const;
        
        
        void               SetCode (const char* const a_code);
        const std::string& Code    () const;
    };

    /**
     * @brief Retrieve the boxed user message
     * @return The user message
     */
    inline const char* Exception::Message () const
    {
        return message_;
    }

    /**
     * @brief Retrieve the source file name of the fault thrower
     * @return Source file name
     */
    inline const char* Exception::File () const
    {
        return file_;
    }

    /**
     * @brief Retrieve the line number of the fault thrower
     * @return line number
     */
    inline int Exception::Line () const
    {
        return line_;
    }

    inline bool Exception::IsNull ()
    {
        return is_null_;
    }

    inline void Exception::SetNull (bool a_is_null)
    {
        is_null_ = a_is_null;
    }
    
    /**
     * @brief Set exception code.
     *
     * @param Exception code, string representation.
     */
    inline void Exception::SetCode (const char *const a_code)
    {
        code_ = a_code;
    }
    
    /**
     * @return The exception code.
     */
    inline const std::string& Exception::Code () const
    {
        return code_;
    }

} // namespace osal

#endif // NRS_OSAL_EXCEPTION_H

