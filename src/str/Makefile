
# Makefile
#
# Copyright (C) 2014 Francesco Abbate
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or (at
# your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#

include ../makeconfig
include ../makedefs

LIBSTR_A = libstr.a

PROJECT_INCLUDES = -I..

INCLUDES += $(PROJECT_INCLUDES)
LIBS +=

LIBSTR_C_SRC_FILES = str.c
LIBSTR_OBJ_FILES := $(LIBSTR_C_SRC_FILES:%.c=%.o)

DEP_FILES := $(LIBSTR_C_SRC_FILES:%.c=.deps/%.P)

DEPS_MAGIC := $(shell mkdir .deps > /dev/null 2>&1 || :)

TARGETS = $(LIBSTR_A)

all: $(TARGETS)

$(LIBSTR_A): $(LIBSTR_OBJ_FILES)
	$(E) archive $@
	$(Q)$(AR) $@ $?
	$(Q)$(RANLIB) $@

clean:
	rm -fr *.o *.a

include ../makerules

.PHONY: clean

-include $(DEP_FILES)
