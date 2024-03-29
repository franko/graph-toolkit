
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

include makeconfig

AR = ar rcu
RANLIB = ranlib

ifneq (,$(findstring Windows,$(OS)))
  HOST_SYS= Windows
else
  HOST_SYS:= $(shell uname -s)
  ifneq (,$(findstring CYGWIN,$(TARGET_SYS)))
    HOST_SYS= Windows
  endif
endif

CFLAGS += -fvisibility=hidden
CXXFLAGS += -fvisibility=hidden
ifeq ($(strip $(DEBUG)), yes)
  CFLAGS += -g -Wall
  CXXFLAGS += -g -Wall
  DEFS += -LIBGRAPH_DEBUG
else
  CFLAGS += -O2 -fomit-frame-pointer -ffast-math -Wall
  CXXFLAGS += -O2 -fno-exceptions -ffast-math -fno-rtti -Wall
endif

PTHREAD_DEFS += -D_REENTRANT
CPP_SUP_LIBS = -lsupc++

ifeq ($(strip $(DISABLE_GAMMA_CORR)), yes)
  DEFS += -DDISABLE_GAMMA_CORR
endif

ifeq ($(strip $(DISABLE_SUBPIXEL_LCD)), yes)
  DEFS += -DDISABLE_SUBPIXEL_AA
endif

# GWH (for OS X): pkg-config will include "-Wl,-rpath,/opt/local/lib" in AGG_LIBS.
# If you don't include that, the code won't run unless you first do:
#   export DYLD_FALLBACK_LIBRARY_PATH=/opt/local/lib

AGG_INCLUDES = $(shell pkg-config libagg --cflags)
AGG_LIBS = $(shell pkg-config libagg --libs)

FREETYPE_INCLUDES = $(shell pkg-config freetype2 --cflags)
FREETYPE_LIBS = $(shell pkg-config freetype2 --libs)

LUA_INCLUDES = $(shell pkg-config --cflags $(LUA))
LUA_LIBS = $(shell pkg-config --libs $(LUA))

PTHREAD_DEFS += -pthread
PTHREAD_LIBS = -lpthread

INCLUDES += $(LUA_INCLUDES) $(FREETYPE_INCLUDES) $(AGG_INCLUDES)
LIBS += $(FREETYPE_LIBS) $(AGG_LIBS) $(LUA_LIBS) $(PTHREAD_LIBS) $(CPP_SUP_LIBS)
DEFS += $(PTHREAD_DEFS)

ifeq ($(strip $(LUA)),lua5.4)
  INCLUDES += -DGRAPH_TK_USE_LUA54
endif

ifeq ($(HOST_SYS),Windows)
  DYNCC = $(CC)
  DYNCXX = $(CXX)
  DLLINK = $(CC) -shared
  LDFLAGS += -fvisibility=hidden
  LIBS += -lgdi32

  DISPLAY_SUFFIX = win32
  LIBGRAPH_SO = graphcore.dll
else
  X11_INCLUDES = $(shell pkg-config x11 --cflags)
  X11_LIBS = $(shell pkg-config x11 --libs)

  INCLUDES += $(X11_INCLUDES)
  LIBS += $(X11_LIBS)

  DYNCC = $(CC) -fPIC
  DYNCXX = $(CXX) -fPIC
  DLLINK = $(CC) -shared
  LDFLAGS += -fvisibility=hidden

  ifeq ($(HOST_SYS),Linux)
    # See: https://stackoverflow.com/questions/2222162/how-to-apply-fvisibility-option-to-symbols-in-static-libraries
    # The following flag is to consider hidden all the symbols defined in static libraries we use
    # to link the shared library.
    LDFLAGS += -Wl,--exclude-libs,ALL
  endif

  ifeq ($(HOST_SYS),Darwin)
    LDFLAGS += -Wl,-E
  endif

  DISPLAY_SUFFIX = x11
  LIBGRAPH_SO = graphcore.so
endif

LIBGRAPH_A = libgraphcore.a

ifeq ($(HOST_SYS),Windows)
  PLATFORM_SRC_FILES = agg_platform_support_win32.cpp agg_win32_bmp.cpp
else
  ifeq ($(HOST_SYS),Darwin)
    DEFS += -DDARWIN_MACOSX
  endif
  PLATFORM_SRC_FILES = agg_platform_support_x11.cpp
endif

PLOT_CSRC_FILES = str.c gs-types.c lua-utils.c lua-properties.c lua-compat.c
PLOT_CPPSRC_FILES = gamma.cpp printf_check.cpp utils.cpp \
	window_registry.cpp fonts_search_$(DISPLAY_SUFFIX).cpp \
	$(PLATFORM_SRC_FILES) image_write_$(DISPLAY_SUFFIX).cpp \
	fonts.cpp agg_font_freetype.cpp plot.cpp plot-auto.cpp units.cpp \
	colors.cpp markers.cpp draw_svg.cpp canvas_svg.cpp lua-draw.cpp\
	lua-text.cpp text.cpp agg-parse-trans.cpp lua-plot.cpp \
	bitmap-plot.cpp lua-graph.cpp canvas-window.cpp window.cpp
PLOT_OBJ_FILES := $(PLOT_CSRC_FILES:%.c=%.o) $(PLOT_CPPSRC_FILES:%.cpp=%.o)

DEP_FILES := $(PLOT_CPPSRC_FILES:%.cpp=.deps/%.P) $(PLOT_CSRC_FILES:%.c=.deps/%.P)

DEPS_MAGIC := $(shell mkdir .deps > /dev/null 2>&1 || :)

ifeq ($(strip $(BUILD)), dynamic)
  COMPILE = $(DYNCC) $(CFLAGS) $(DEFS) $(INCLUDES)
  CXXCOMPILE = $(DYNCXX) $(CXXFLAGS) $(DEFS) $(INCLUDES)

  TARGETS = $(LIBGRAPH_SO)
else
  COMPILE = $(CC) $(CFLAGS) $(DEFS) $(INCLUDES)
  CXXCOMPILE = $(CXX) $(CXXFLAGS) $(DEFS) $(INCLUDES)

  TARGETS = $(LIBGRAPH_A)
endif

all: $(TARGETS)

$(LIBGRAPH_SO): $(PLOT_OBJ_FILES)
	$(DLLINK) $(LDFLAGS) $(PLOT_OBJ_FILES) -o $@ $(LIBS)
	strip --strip-unneeded $@

$(LIBGRAPH_A): $(PLOT_OBJ_FILES)
	@echo Archive $@
	@$(AR) $@ $?
	@$(RANLIB) $@

clean:
	rm -fr *.o *.a *.so *.dll $(TARGETS)

%.o: %.cpp
	@echo Compiling $<
	@$(CXXCOMPILE) -Wp,-MMD,.deps/$(*F).pp -c $<
	@-cp .deps/$(*F).pp .deps/$(*F).P; \
	tr ' ' '\012' < .deps/$(*F).pp \
          | sed -e 's/^\\$$//' -e '/^$$/ d' -e '/:$$/ d' -e 's/$$/ :/' \
            >> .deps/$(*F).P; \
	rm .deps/$(*F).pp

%.o: %.c
	@echo Compiling $<
	@$(COMPILE) -Wp,-MMD,.deps/$(*F).pp -c $<
	@-cp .deps/$(*F).pp .deps/$(*F).P; \
	tr ' ' '\012' < .deps/$(*F).pp \
          | sed -e 's/^\\$$//' -e '/^$$/ d' -e '/:$$/ d' -e 's/$$/ :/' \
            >> .deps/$(*F).P; \
	rm .deps/$(*F).pp

.PHONY: clean all

-include $(DEP_FILES)
