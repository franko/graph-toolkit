project('graph-toolkit', ['c', 'cpp'], version : '1.0', default_options : ['c_std=gnu11', 'cpp_std=c++14'])

cc = meson.get_compiler('c')

graph_toolkit_lua = get_option('lua')

lua_dep = dependency(graph_toolkit_lua)
freetype_dep = dependency('freetype2')
thread_dep = dependency('threads')

libagg_project = subproject('libagg')
libagg_dep = libagg_project.get_variable('libagg_dep')

libm  = cc.find_library('m',  required: false)
libdl = cc.find_library('dl', required: false)

graph_toolkit_deps = [ libagg_dep, freetype_dep, lua_dep, thread_dep, libm, libdl ]

if host_machine.system() != 'windows'
  graph_toolkit_deps += dependency('x11')
endif

graph_toolkit_c_args = [ ]

subdir('src')

install_data('init.lua', install_dir : 'share/lua/5.4/graph')
install_data('contour.lua', install_dir : 'share/lua/5.4/graph')
