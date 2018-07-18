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

include common.mk

OBJECTS  = \
		   $(OSAL_SRC:.cc=.o) \
		   $(RAGEL_SRC:.rl=.o)

LIB_NAME:=libosal.a
A_FILE:=$(OUT_DIR_FOR_TARGET)/$(LIB_NAME)

all: mk_out_dir ragel $(OBJECTS)
	@ar rcs $(A_FILE) $(OBJECTS)
	@echo "* [$(TARGET)] $(A_FILE) ~> done"

clean_lib:
	@echo "* [clean] $(LIB_NAME)..."
	@rm -f $(OBJECTS)
	@rm -f $(A_FILE)

.SECONDARY:
