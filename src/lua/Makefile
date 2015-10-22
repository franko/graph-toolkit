
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
include ../makegraphdefs

ifeq ($(HOST_SYS),Windows)
  LUA_INCLUDES = -IC:/fra/src/luajit-2.0/src
  LUA_LIBS = -LC:/fra/src/luajit-2.0/src -llua51

  LIBGRAPH_SO = graphcore.dll
else
  LUA_INCLUDES = $(shell pkg-config --cflags $(LUA))
  LUA_LIBS = $(shell pkg-config --libs $(LUA))

  LIBGRAPH_SO = libgraphcore.so
endif

PROJECT_INCLUDES = -I.. -I../str -I../core
PROJECT_LIBS = ../core/libgraph.a ../str/libstr.a

INCLUDES += $(PROJECT_INCLUDES) $(FREETYPE_INCLUDES) $(X11_INCLUDES) $(AGG_INCLUDES) $(LUA_INCLUDES)
LIBS += $(FREETYPE_LIBS) $(AGG_LIBS) $(X11_LIBS) $(PTHREAD_LIBS) $(LUA_LIBS)

GRAPH_C_SRC_FILES = gs-types.c lua-properties.c lua-utils.c
GRAPH_CPP_SRC_FILES = agg-parse-trans.cpp lua-graph.cpp split_area.cpp \
	lua-colors.cpp lua-plot.cpp window_lua.cpp \
	lua-draw.cpp lua-text.cpp window_registry.cpp

GRAPH_OBJ_FILES := $(GRAPH_C_SRC_FILES:%.c=%.o) $(GRAPH_CPP_SRC_FILES:%.cpp=%.o)

DEP_FILES := $(GRAPH_C_SRC_FILES:%.c=.deps/%.P) $(GRAPH_CPP_SRC_FILES:%.cpp=.deps/%.P)

DEPS_MAGIC := $(shell mkdir .deps > /dev/null 2>&1 || :)

TARGETS = $(LIBGRAPH_SO)

all: $(TARGETS)

$(LIBGRAPH_SO): $(GRAPH_OBJ_FILES)
	$(E) linking $@
	$(Q)$(DLLINK) $(GRAPH_OBJ_FILES) $(PROJECT_LIBS) -o $@ $(LIBS) -lsupc++
	$(Q)strip --strip-unneeded $@

clean:
	rm -fr *.o *.so

include ../makerules

.PHONY: clean

-include $(DEP_FILES)
