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

PLATFORM=$(shell uname -s)

####################
# Set library type
####################
ifndef LIB_TYPE
  LIB_TYPE:="static"
endif
override LIB_TYPE:=$(shell echo $(LIB_TYPE) | tr A-Z a-z)

# validate library type
ifeq (static, $(LIB_TYPE))
#
else
$(error "Don't know how to build for target $(LIB_TYPE) ")
endif

####################
# Set target type
####################
ifeq (Darwin, $(PLATFORM))
 ifndef TARGET
   TARGET:=Debug
 else
   override TARGET:=$(shell echo $(TARGET) | tr A-Z a-z)
   $(eval TARGET_E:=$$$(TARGET))
   TARGET_E:=$(shell echo $(TARGET_E) | tr A-Z a-z )
   TARGET_S:=$(subst $(TARGET_E),,$(TARGET))
   TARGET_S:=$(shell echo $(TARGET_S) | tr a-z A-Z )
   TMP_TARGET:=$(TARGET_S)$(TARGET_E)
   override TARGET:=$(TMP_TARGET)
 endif
 OUT_DIR = ./out/darwin
else
 ifndef TARGET
   TARGET:=debug
 else
   override TARGET:=$(shell echo $(TARGET) | tr A-Z a-z)
 endif
 OUT_DIR = ./out/linux
endif
TARGET_LC:=$(shell echo $(TARGET) | tr A-Z a-z)

# validate target
ifeq (release, $(TARGET_LC))
  #
else
  ifeq (debug, $(TARGET_LC))
    #
  else
    $(error "Don't know how to build for target $(TARGET_LC) ")
  endif
endif

OUT_DIR_FOR_TARGET := $(OUT_DIR)/$(TARGET)

OSAL_HEADERS_DIR := \
		./src

OSAL_SRC := \
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
						./src/osal/utils/utf8_utils

OSAL_RAGEL_SRC := \
                  ./src/osal/osal_date.rl

RAGEL_SRC := \
            $(OSAL_RAGEL_SRC)

ifndef ICU_INCLUDE_DIRS
  ICU_INCLUDE_DIRS := /usr/local/opt/icu4c/include
endif

# includes
INCLUDE_DIRS := \
               -I /usr/local/include  \
	       -I $(OSAL_HEADERS_DIR) \
               -I $(ICU_INCLUDE_DIRS)

# compiler flags
C           := gcc
CXX         := g++
CXXFLAGS    := -std=gnu++11 $(INCLUDE_DIRS) -c -Wall
CFLAGS      := $(INCLUDE_DIRS) -c -Wall
RAGEL_FLAGS :=
RAGEL 		:= $(shell which ragel)

ifeq ($(TARGET),release)
  CXXFLAGS += -g -O2 -DNDEBUG
else
  CXXFLAGS += -g -O0 -DDEBUG
endif

ifneq (static, $(LIB_TYPE))
$(error not ready to build $(LIB_TYPE) library)
else
	CFLAGS+="-static"
	CXXFLAGS+="-static"
endif

# VERSION :=$(shell cat version)

### ###

info-third-party:
	@echo "---"
	@echo "- casper-osal/common.mk"
	@echo "- ICU_INCLUDE_DIRS: $(ICU_INCLUDE_DIRS)"
	@echo "-"

### ###

# out dir
mk_out_dir:
	@mkdir -p $(OUT_DIR_FOR_TARGET)
	@echo "* [$(TARGET)] ${OUT_DIR_FOR_TARGET}..."

# .rl - generate .cc from .rl
ragel: $(RAGEL_SRC:.rl=.cc)
	@echo "* [$(TARGET)] rl  ~> done"

# bison
%.cc:%.yy
	@echo "* [$(TARGET)] yy  $< ~> $@ ..."
	@$(YACC) $< -v --locations -o $@

# ragel
%.cc:%.rl
	@echo "* [$(TARGET)] rl  $< ..."
	@$(RAGEL) $(RAGEL_FLAGS) $< -G2 -o $@

# c++
.c.o:
	@echo "* [$(TARGET)] c   $< ..."
	@$(C) $(CFLAGS) $< -o $@

# c++
.cc.o:
	@echo "* [$(TARGET)] cc  $< ..."
	@$(CXX) $(CXXFLAGS) $< -o $@

# c++
.cpp.o:
	@echo "* [$(TARGET)] cpp $< ..."
	@$(CXX) $(CXXFLAGS) $< -o $@

# clean
clean: clean_lib ragel-clean
	@echo "* [common-clean]..."
	@find . -name "*~" -delete
	@echo "* [common-clean] done..."

# ragel
ragel-clean:
	@echo "* [ragel-clean]..."
	@rm -f $(RAGEL_SRC:.rl=.cc)
	@echo "* [ragel-clean] done..."
