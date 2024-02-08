graph_toolkit_src_files = [
  'lua-compat.c',
  'str.c',
  'gs-types.c',
  'lua-utils.c',
  'lua-properties.c',
  'gamma.cpp',
  'printf_check.cpp',
  'utils.cpp',
  'window_registry.cpp',
  'fonts.cpp',
  'agg_font_freetype.cpp',
  'plot.cpp',
  'plot-auto.cpp',
  'units.cpp',
  'colors.cpp',
  'markers.cpp',
  'draw_svg.cpp',
  'canvas_svg.cpp',
  'lua-draw.cpp',
  'lua-text.cpp',
  'text.cpp',
  'agg-parse-trans.cpp',
  'lua-plot.cpp',
  'bitmap-plot.cpp',
  'lua-graph.cpp',
  'canvas-window.cpp',
  'window.cpp',
]

if not get_option('gamma-correction')
  graph_toolkit_c_args += '-DDISABLE_GAMMA_CORR'
endif

if not get_option('subpixel-lcd')
  graph_toolkit_c_args += '-DDISABLE_SUBPIXEL_AA'
endif

if lua_name == 'lua5.4'
  graph_toolkit_c_args += '-DGRAPH_TK_USE_LUA54'
endif

if host_machine.system() == 'windows'
  graph_toolkit_src_files += [ 'fonts_search_win32.cpp', 'image_write_win32.cpp' ]
  graph_toolkit_src_files += [ 'agg_platform_support_win32.cpp', 'agg_win32_bmp.cpp', 'agg_win32_bmp.cpp' ]
else
  # Besides windows we support only X11 even on macOS
  if host_machine.system() == 'darwin'
    graph_toolkit_c_args += '-DDARWIN_MACOSX'
  endif
  graph_toolkit_src_files += [ 'fonts_search_x11.cpp', 'image_write_x11.cpp' ]
  graph_toolkit_src_files += [ 'agg_platform_support_x11.cpp' ]
endif

graph_toolkit_link_args = [ ]
if cc.get_id() == 'gcc'
  graph_toolkit_link_args += ['-static-libgcc', '-static-libstdc++']
endif

if host_machine.system() == 'linux'
  # See: https://stackoverflow.com/questions/2222162/how-to-apply-fvisibility-option-to-symbols-in-static-libraries
  # The following flag is to consider hidden all the symbols defined in static libraries we use
  # to link the shared library.
  graph_toolkit_link_args += [ '-Wl,--exclude-libs,ALL' ]
endif

libgraph = shared_library('graphcore',
  graph_toolkit_src_files,
  dependencies: graph_toolkit_deps,
  c_args : graph_toolkit_c_args,
  cpp_args : graph_toolkit_c_args,
  link_args : graph_toolkit_link_args,
  gnu_symbol_visibility : 'hidden',
  name_prefix : '',
  install_dir : 'lib' / lua_modules_dir,
  install : true,
)

