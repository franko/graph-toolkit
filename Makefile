
PREFIX = /usr
DEST_DIR =

ifeq ($(strip $(DEST_DIR)), )
  DEST_PREFIX = $(PREFIX)
else
  DEST_PREFIX = $(DEST_DIR)/$(PREFIX)
endif

GRAPH_LUA_SRC = init.lua contour.lua

HOST_CP = cp
HOST_RM = rm -f
CP_REL = cp --parents

LUA_PATH = $(DEST_PREFIX)/share/lua/5.1
LUA_DLLPATH = $(DEST_PREFIX)/lib/lua/5.1
SYSTEM_LIBPATH = $(DEST_PREFIX)/lib
DEBIAN = debian_build/$(PREFIX)
WIN_INSTALL_DIR = windows_build

all:
	$(MAKE) -C src

clean:
	$(MAKE) -C src clean

install:
	$(MAKE) -C src
	mkdir -p $(LUA_PATH)
	mkdir -p $(LUA_DLLPATH)
	cp src/libgraphcore.so $(LUA_DLLPATH)/graphcore.so
	cp src/libnatwin.so $(LUA_DLLPATH)/natwin.so
	ln -s $(LUA_DLLPATH)/graphcore.so $(SYSTEM_LIBPATH)/libgraphcore.so
	$(CP_REL) $(GRAPH_LUA_SRC) $(LUA_PATH)

debian:
	$(MAKE) -C src
	rm -fr debian_build
	rm -fr lua-graph-toolkit*.deb
	mkdir -p $(DEBIAN)/share/lua/5.1
	mkdir -p $(DEBIAN)/share/lua/5.1/graph
	mkdir -p $(DEBIAN)/lib/lua/5.1
	cp src/libgraphcore.so $(DEBIAN)/lib/lua/5.1/graphcore.so
	cp src/libnatwin.so $(DEBIAN)/lib/lua/5.1/natwin.so
	ln -s lua/5.1/graphcore.so $(DEBIAN)/lib/libgraphcore.so
	cp $(GRAPH_LUA_SRC) $(DEBIAN)/share/lua/5.1/graph
	fakeroot bash debian/build.sh $(VERSION)

win_install:
	$(MAKE) -C src
	mkdir -p $(WIN_INSTALL_DIR)
	mkdir -p $(WIN_INSTALL_DIR)/lua
	mkdir -p $(WIN_INSTALL_DIR)/lua/graph
	cp src/graphcore.dll $(WIN_INSTALL_DIR)
	cp src/natwin.dll $(WIN_INSTALL_DIR)
	cp $(GRAPH_LUA_SRC) $(WIN_INSTALL_DIR)/lua/graph
	cp -r examples $(WIN_INSTALL_DIR)

.PHONY: clean all install debian win_install
