
TESTS_C_SRC_FILES =
TESTS_CPP_SRC_FILES = plot1.cpp
TESTS_EXE_FILES := $(TESTS_CPP_SRC_FILES:%.cpp=%) $(TESTS_C_SRC_FILES:%.cpp=%)

X11_INCLUDES = $(shell pkg-config x11 --cflags)
X11_LIBS = $(shell pkg-config x11 --libs)

AGG_INCLUDES = $(shell pkg-config libagg --cflags)
AGG_LIBS = $(shell pkg-config libagg --libs)

FREETYPE_INCLUDES = $(shell pkg-config freetype2 --cflags)
FREETYPE_LIBS = $(shell pkg-config freetype2 --libs)

LUA_INCLUDES = $(shell pkg-config --cflags luajit)
LUA_LIBS = $(shell pkg-config --libs luajit)

INCLUDES = -I../src $(LUA_INCLUDES) $(X11_INCLUDES) $(AGG_INCLUDES) $(FREETYPE_INCLUDES)

CC = gcc
CXX = g++

CFLAGS = -g -Wall
CXXFLAGS = -g -Wall

CXXCOMPILE = $(CXX) $(CXXFLAGS) $(INCLUDES)
COMPILE = $(CC) $(CFLAGS) $(INCLUDES)

all: $(TESTS_EXE_FILES)

DEP_FILES := $(TESTS_CPP_SRC_FILES:%.cpp=.deps/%.P) $(TESTS_C_SRC_FILES:%.c=.deps/%.P)

DEPS_MAGIC := $(shell mkdir .deps > /dev/null 2>&1 || :)

clean:
	rm -fr *.o

%: %.o
	$(CC) -o $@ $< -L../src -lgraphcore -lm

%.o: %.cpp
	@echo Compiling $<
	$(CXXCOMPILE) -Wp,-MMD,.deps/$(*F).pp -c $<
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

.PHONY: clean

-include $(DEP_FILES)
