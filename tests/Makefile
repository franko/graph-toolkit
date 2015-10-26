include ../src/makeconfig
include ../src/makedefs
include ../src/makegraphdefs

TESTS_C_SRC_FILES =
TESTS_CPP_SRC_FILES = plot1.cpp plot1_svg.cpp
TESTS_EXE_FILES := $(TESTS_CPP_SRC_FILES:%.cpp=%) $(TESTS_C_SRC_FILES:%.cpp=%)

PROJECT_INCLUDES = -I../src -I../src/core -I../src/str
PROJECT_LIBS = ../src/core/libgraph.a ../src/str/libstr.a

INCLUDES = $(PROJECT_INCLUDES) $(X11_INCLUDES) $(AGG_INCLUDES) $(FREETYPE_INCLUDES)
LIBS += $(FREETYPE_LIBS) $(AGG_LIBS) $(X11_LIBS) $(PTHREAD_LIBS)

all: $(TESTS_EXE_FILES)

DEP_FILES := $(TESTS_CPP_SRC_FILES:%.cpp=.deps/%.P) $(TESTS_C_SRC_FILES:%.c=.deps/%.P)

DEPS_MAGIC := $(shell mkdir .deps > /dev/null 2>&1 || :)

clean:
	rm -fr *.o $(TESTS_EXE_FILES)

%: %.o
	$(E) compiling $@
	$(Q)$(CXX) -o $@ $< $(PROJECT_LIBS) $(LIBS) -lm

include ../src/makerules

.PHONY: all clean

-include $(DEP_FILES)
