
# Makefile
#
# Copyright (C) 2014-2024 Francesco Abbate
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

include src/makeconfig

VERSION = 1.1

PREFIX = /usr
DEST_DIR =

ifeq ($(strip $(DEST_DIR)), )
  DEST_PREFIX = $(PREFIX)
else
  DEST_PREFIX = $(DEST_DIR)/$(PREFIX)
endif

GRAPH_LUA_SRC = init.lua contour.lua

LUA_PATH = $(DEST_PREFIX)/share/lua/$(LUA_VERSION)
LUA_DLLPATH = $(DEST_PREFIX)/lib/lua/$(LUA_VERSION)

all:
	$(MAKE) -C src

clean:
	$(MAKE) -C src clean

install:
	$(MAKE) -C src
	mkdir -p $(LUA_PATH)/graph
	mkdir -p $(LUA_DLLPATH)
	cp src/graphcore.so $(LUA_DLLPATH)/graphcore.so
	cp $(GRAPH_LUA_SRC) $(LUA_PATH)/graph

.PHONY: clean all install
