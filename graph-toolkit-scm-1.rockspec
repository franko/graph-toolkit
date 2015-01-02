package = "graph-toolkit"
version = "scm-1"
source = {
   dir = "graph-toolkit-master",
   url = "https://github.com/franko/graph-toolkit/archive/master.zip",
}
description = {
   summary = "Lua Graphics Toolkit",
   detailed = [[
      The Lua Graphics Toolkit is a Lua module for creating plots and graphical
      animations using a simple Lua interface. The module implements the core
      functions in C and C++ using Lua's C API. A second layer of functions are
      implemented in Lua.
   ]],
   homepage = "http://franko.github.io/graph-toolkit/",
   license = "GPL-3"
}
dependencies = {
   "lua >= 5.1"
}
external_dependencies = {
   AGG = {
      header = "agg2/agg_basics.h"
   },
   FREETYPE = {
      header = "freetype2/ft2build.h",
      library = "freetype"
   },
   platforms = {
      unix = {
         X11 = {
            header = "X11/Xlib.h",
            library = "X11"
         }
      },
      macosx = {
         X11 = {
            header = "X11/Xlib.h",
            library = "X11"
         }
      }
   }
}
build = {
   type = "make",
   build_variables = {
     CFLAGS = "$(CFLAGS) -I$(LUA_INCDIR)",
     CXXFLAGS = "$(CXXFLAGS) -I$(LUA_INCDIR)",
   },
   install_variables = {
      PREFIX = "$(PREFIX)",
      LUA_PATH = "$(LUADIR)",
      LUA_DLLPATH = "$(LIBDIR)"
   },
   copy_directories = { "doc" },
}
