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

THIS_DIR := $(abspath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
ifeq (casper-osal, $(shell basename $(THIS_DIR)))
  PACKAGER_DIR := $(abspath $(THIS_DIR)/../casper-packager)
else
  PACKAGER_DIR := $(abspath $(THIS_DIR)/..)
endif

include $(PACKAGER_DIR)/common/c++/settings.mk

PROJECT_SRC_DIR     := $(ROOT_DIR)/casper-osal
EXECUTABLE_NAME     := 
EXECUTABLE_MAIN_SRC :=
LIBRARY_TYPE        := static
ifeq (true, $(V8_DEP_ON))
  LIBRARY_NAME := libosal-v8.a
else ifeq (true, $(ICU_STAND_ALONE_DEP_ON))
  LIBRARY_NAME := libosal-icu.a
else
  LIBRARY_NAME := libosal.a
endif
VERSION             := $(shell cat $(PROJECT_SRC_DIR)/VERSION)
CHILD_CWD           := $(THIS_DIR)
CHILD_MAKEFILE      := $(firstword $(MAKEFILE_LIST))

############################                                                                                                                                                                                                                  
# OSAL VARIABLES                                                                                                                                                                                                                              
############################    

include $(PROJECT_SRC_DIR)/common.mk

############################
# COMMON REQUIRED VARIABLES
############################

CC_SRC := \
  $(OSAL_CC_SRC)

RAGEL_SRC := \
  $(OSAL_RAGEL_SRC)

INCLUDE_DIRS := \
  -I $(PROJECT_SRC_DIR)/src \
  -I $(ICU_INCLUDE_DIRS)

OBJECTS := \
  $(OSAL_CC_SRC:.cc=.o) \
  $(RAGEL_SRC:.rl=.o)

include $(PACKAGER_DIR)/common/c++/common.mk

OSAL_DEPS=
ifeq (true, $(ICU_STAND_ALONE_DEP_ON))
  CXXFLAGS+=-DCASPER_REQUIRE_STANDALONE_ICU=1
  CFLAGS+=-DCASPER_REQUIRE_STANDALONE_ICU=1
  OSAL_DEPS=casper-osal-icu-dep-on
endif

set-dependencies: $(OSAL_DEPS)

all: lib

.SECONDARY:
