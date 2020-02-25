/**
 * @file posix_thread_helper.cc
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

#include "osal/posix/posix_thread_helper.h"

#include "osal/exception.h"

#include <signal.h>  // sigaddset
#include <string.h>

const osal::posix::ThreadHelper::ThreadID osal::posix::ThreadHelper::k_invalid_thread_id_ = 0;

osal::posix::ThreadHelper::ThreadID osal::posix::ThreadHelper::main_thread_id_            = osal::posix::ThreadHelper::k_invalid_thread_id_;

/**
 * @brief Static helper method to block current thead signals.
 *
 * @param a_signals Set of signals to block.
 *
 * @remarks sigemptyset and sigaddset are async-signal-safe according to the POSIX standard.
 */
void osal::posix::ThreadHelper::BlockSignals (const std::set<int>& a_signals)
{
  sigset_t mask;
  sigemptyset (&mask);
  for ( auto signal : a_signals ) {
    sigaddset(&mask, signal);
  }
  const int rv = pthread_sigmask(SIG_BLOCK, &mask, nullptr);
  if ( 0 != rv ) {
    throw OSAL_EXCEPTION("Unable to block thread signals: %d - %s!", rv, strerror(rv));
  }
}
