#pragma once
/**
 * @file swatch.h general purpose utility cronograph
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
#ifndef NRS_OSAL_UTIL_SWATCH_H
#define NRS_OSAL_UTIL_SWATCH_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>

namespace osal
{
	namespace utils
	{
    	/**
    	 * @brief general purpose utility cronograph
    	 * Instances of this class measure the time between the #start()
    	 * and #stop() member invocations.
    	 */
    	class Swatch
    	{
    	public:
    		enum { ONE_SECOND = 1000000L }; //!< in microseconds

    	protected:

    		struct timeval stime_;    //!< Start time keeper
    		struct timeval duration_; //!< Duration keeper
    		bool 	       running_;

    	public: // methods
    		/**
    		 * @brief Constructor, elapsed time starts at 0 and timer is stopped
    		 */
    		Swatch ()
    		{
    			duration_.tv_sec = 0;
    			duration_.tv_usec = 0;
    			stime_.tv_sec = 0;
    			stime_.tv_usec = 0;
    			running_ = false;
    		}

    		/**
    		 * @brief Start chronograph
    		 */
    		void Start ()
    		{
    			gettimeofday(&stime_, NULL);
    			duration_.tv_sec = 0;
    			duration_.tv_usec = 0;
    			running_ = true;
    		}

    		/**
    		 * @brief Stop chronograph
    		 * @return accumulated time interval in microseconds
    		 */
    		unsigned long Stop ()
    		{
    			if ( running_ == true ) {
    				UpdateDuration();
    				running_ = false;
    			}
    			return GetTime();
    		}

    		/**
    		 * @brief Get an intermediate (lap) time while continuing global counter.
    		 * @return Time since last call to #start() or #lapTime().
    		 */
    		unsigned long LapTime ()
    		{
    			unsigned long rv = 0;

    			if ( running_ == true ) {
    				UpdateDuration();
    				rv = Ticks();
    				gettimeofday(&stime_, NULL);
    			} else {
    				Start();
    			}
    			return rv;
    		}

    		/**
    		 * @brief Reset the accumulated time count
    		 */
    		void Reset ()
    		{
    			duration_.tv_sec = 0;
    			duration_.tv_usec = 0;
    		}

    		/**
    		 * @brief Return the accumulated time interval in micro seconds
    		 * @return The number of microseconds elapsed between #start() and #stop() calls
    		 */
    		operator unsigned long ()
    		{
    			return duration_.tv_sec * ONE_SECOND + duration_.tv_usec;
    		}

    		/**
    		 * @brief Return the accumulated time interval in micro seconds
    		 * @return The number of microseconds elapsed between #start() and #stop() calls
    		 */
    		unsigned long GetTime ()
    		{
    			return duration_.tv_sec * ONE_SECOND + duration_.tv_usec;
    		}

    		/**
    		 * @brief Return the elapsed ticks since the last call to #start() or #lapTime(),
    		 * the timer continues to run.
    		 * @return The number of microseconds elapsed since the last call to #start() or #lapTime()
    		 */
    		unsigned long Ticks ()
    		{
    			struct timeval now;

    			gettimeofday(&now, NULL);
    			return (now.tv_sec - stime_.tv_sec) * ONE_SECOND +
    				now.tv_usec - stime_.tv_usec;
    		}

    		/**
    		 * @brief Check if the timer is running
    		 * @return @li true if the chronograph is running;
    		 *         @li false chrono stopped;
    		 */
    		bool IsRunning ()
    		{
    			return running_;
    		}

    	private:

    		/**
    		 * @brief Update the accumulated time interval
    		 *
    		 * The net result is:
    		 *   <code>duration = duration + delta <=> duration = duration + (stime - now)</code>
    		 * where:
    		 *   @li <code>now</code> is the current time
    		 *   @li <code>stime</code> is the start of the time interval
    		 */
    		void UpdateDuration ()
    		{
    			struct timeval now;
    			struct timeval diff;

    			gettimeofday(&now, NULL);
    			while (now.tv_usec >= ONE_SECOND){
    				now.tv_usec -= ONE_SECOND;
    				now.tv_sec++;
    			}
    			diff.tv_sec  = now.tv_sec - stime_.tv_sec;
    			diff.tv_usec = now.tv_usec - stime_.tv_usec;
    			duration_.tv_sec  += diff.tv_sec;
    			duration_.tv_usec  += diff.tv_usec;
    			while ( duration_.tv_usec >= ONE_SECOND){
    				duration_.tv_usec -= ONE_SECOND;
    				duration_.tv_sec++;
    			}
    		}
    	}; // class Swatch

	} // namespace utils
} // namespace osal

#endif // NRS_OSAL_UTIL_SWATCH_H
