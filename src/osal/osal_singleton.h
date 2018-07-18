#pragma once
/**
 * @file singleton.h - NOT Thread safe singleton
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

#ifndef NRS_OSAL_SINGLETON_H_
#define NRS_OSAL_SINGLETON_H_

namespace osal
{

    template <typename T> class Singleton
    {

    private: // static data

        static T* instance_;

    protected: // constructor

        Singleton ()
        {
            // EMPTY
        }

        ~Singleton()
        {

        }

    private: // operators

        T& operator = (const T& a_singleton)
        {
            if ( &a_singleton != this ) {
                instance_ = a_singleton.instance_;
            }
            return this;
        }

    public: // inline method(s) / function(s) declaration

        static T& GetInstance () __attribute__ ((visibility ("default")))
        {
            if ( nullptr == Singleton<T>::instance_ ) {
                Singleton<T>::instance_ = new T();
            }
            return *Singleton<T>::instance_;
        }

	static void Destroy ()
	{
	  if ( nullptr != Singleton<T>::instance_ ) {
          delete instance_;
	      instance_ = nullptr;
	  }
	}

    public: // Operators Overload

	// delete copy and move constructors and assign operators
	Singleton(Singleton const&)            = delete;        // Copy construct
	Singleton(Singleton&&)                 = delete;        // Move construct
	Singleton& operator=(Singleton const&) = delete;        // Copy assign
	Singleton& operator=(Singleton &&)     = delete;        // Move assign

    }; // end of class Singleton

    template <typename T> T* Singleton<T>::instance_ = nullptr;

} // end of namespace osal

#endif // NRS_OSAL_SINGLETON_H_
