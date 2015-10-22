
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
  DISPLAY_SUFFIX = win32
  PLATFORM_SRC_FILES = agg_platform_support_win32.cpp agg_win32_bmp.cpp agg_win32_bmp.cpp
else
  DISPLAY_SUFFIX = x11
  PLATFORM_SRC_FILES = agg_platform_support_x11.cpp
  ifeq ($(HOST_SYS),Darwin)
    DEFS += -DDARWIN_MACOSX
  endif
endif

LIBGRAPH_A = libgraph.a

PROJECT_INCLUDES = -I.. -I../str

INCLUDES += $(PROJECT_INCLUDES) $(FREETYPE_INCLUDES) $(X11_INCLUDES) $(AGG_INCLUDES)
LIBS += $(FREETYPE_LIBS) $(AGG_LIBS) $(X11_LIBS) $(PTHREAD_LIBS)

PLOT_CPPSRC_FILES = gamma.cpp printf_check.cpp utils.cpp \
	graph_locks.cpp fonts_search_$(DISPLAY_SUFFIX).cpp \
	$(PLATFORM_SRC_FILES) image_write_$(DISPLAY_SUFFIX).cpp \
	fonts.cpp agg_font_freetype.cpp plot.cpp plot-auto.cpp units.cpp \
	colors.cpp markers.cpp draw_svg.cpp canvas_svg.cpp \
	text.cpp bitmap-plot.cpp canvas-window.cpp window.cpp
PLOT_OBJ_FILES := $(PLOT_CPPSRC_FILES:%.cpp=%.o)

DEP_FILES := $(PLOT_CPPSRC_FILES:%.cpp=.deps/%.P)

DEPS_MAGIC := $(shell mkdir .deps > /dev/null 2>&1 || :)

TARGETS = $(LIBGRAPH_A)

all: $(TARGETS)

$(LIBGRAPH_A): $(PLOT_OBJ_FILES)
	$(E) archive $@
	$(Q)$(AR) $@ $?
	$(Q)$(RANLIB) $@

clean:
	rm -fr *.o *.a

include ../makerules

.PHONY: clean

-include $(DEP_FILES)
