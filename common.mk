#
# Copyright (c) 2011-2019 Cloudware S.A. All rights reserved.
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

############################
# OSAL VARIABLES
############################

OSAL_CC_SRC := \
  $(PROJECT_SRC_DIR)/src/osal/base_file.cc                           \
  $(PROJECT_SRC_DIR)/src/osal/exception.cc                           \
  $(PROJECT_SRC_DIR)/src/osal/debug/trace.cc                          \
  $(PROJECT_SRC_DIR)/src/osal/posix/posix_circular_buffer.cc         \
  $(PROJECT_SRC_DIR)/src/osal/posix/posix_circular_buffer_no_mmap.cc \
  $(PROJECT_SRC_DIR)/src/osal/posix/posix_condition_variable.cc      \
  $(PROJECT_SRC_DIR)/src/osal/posix/posix_datagram_socket.cc         \
  $(PROJECT_SRC_DIR)/src/osal/posix/posix_dir.cc                     \
  $(PROJECT_SRC_DIR)/src/osal/posix/posix_file.cc                    \
  $(PROJECT_SRC_DIR)/src/osal/posix/posix_mutex.cc                   \
  $(PROJECT_SRC_DIR)/src/osal/posix/posix_random.cc                  \
  $(PROJECT_SRC_DIR)/src/osal/posix/posix_time.cc                    \
  $(PROJECT_SRC_DIR)/src/osal/utf8_string.cc                         \
  $(PROJECT_SRC_DIR)/src/osal/utils/base_64.cc                       \
  $(PROJECT_SRC_DIR)/src/osal/utils/json_parser_base.cc              \
  $(PROJECT_SRC_DIR)/src/osal/utils/pow10.cc                         \
  $(PROJECT_SRC_DIR)/src/osal/utils/utf8_utils.cc

OSAL_RAGEL_SRC := \
  $(PROJECT_SRC_DIR)/src/osal/osal_date.rl
