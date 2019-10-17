#
# Copyright (c) 2011-2018 Cloudware S.A. All rights reserved.
#
# This file is part of casper-osal.
#
# casper-osal is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# casper-osal is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with osal.  If not, see <http://www.gnu.org/licenses/>.
#

CC_SRC := \
	./src/osal/base_file.cc                           \
	./src/osal/exception.cc                           \
	./src/osal/posix/posix_circular_buffer.cc         \
	./src/osal/posix/posix_circular_buffer_no_mmap.cc \
	./src/osal/posix/posix_condition_variable.cc      \
	./src/osal/posix/posix_datagram_socket.cc         \
	./src/osal/posix/posix_dir.cc                     \
	./src/osal/posix/posix_file.cc                    \
	./src/osal/posix/posix_mutex.cc                   \
	./src/osal/posix/posix_random.cc                  \
	./src/osal/posix/posix_thread_helper.cc           \
	./src/osal/posix/posix_time.cc                    \
	./src/osal/utf8_string.cc                         \
	./src/osal/utils/base_64.cc                       \
	./src/osal/utils/json_parser_base.cc              \
	./src/osal/utils/pow10.cc                         \
	./src/osal/utils/utf8_utils.cc

OSAL_RAGEL_SRC := \
	 ./src/osal/osal_date.rl

RAGEL_SRC := \
            $(OSAL_RAGEL_SRC)

################
# INCLUDE_DIRS
################# includes
INCLUDE_DIRS := \
	-I ./src

################
# OBJECTS
################
OBJECTS := \
           $(CC_SRC:.cc=.o) \
           $(RAGEL_SRC:.rl=.o)

