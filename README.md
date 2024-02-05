Lua Graphics Toolkit
====================

The Lua Graphics Toolkit is a Lua module designed for crafting plots and graphics
animations through an easy-to-use Lua interface.
This module is compatible with both Windows and Linux as a standard Lua module.
Although it can function on macOS, it requires an X Window server.

To ensure high-quality rendering, the module employs the AntiGrain Geometry library.
This allows for exceptional rendering quality via anti-aliasing and subpixel resolution.

It supports Lua versions 5.1, 5.4, and LuaJIT 2.0 or 2.1.

All functions are thoroughly documented in the
[user manual](http://franko.github.io/graph-toolkit/),
and several [examples](https://github.com/franko/graph-toolkit/tree/master/examples)
are available in the git repository.

The toolkit offers a straightforward interface for plotting functions.
However, users can also access detailed control options. These include customizing
graphical elements such as colors, positions, and the rendering pipeline.
Additionally, the module natively supports Bezier curves, thanks to the AGG library.

Further customization is available for axes, allowing for unique labels and even
layered label systems.

Plot legends are also a feature, although with certain restrictions. Currently,
legends cannot be placed *inside* the plot area.

Animations
----------

The module facilitates the creation of animations through a layered mechanism
and several methods for adding and clearing elements from the current layer.

To craft an animation, elements are added to the plot. To generate a new frame,
the current layer is cleared using the `plot:clear()` method, allowing the
addition of new elements for the next frame.
The "sync" property can be set to "false" to accumulate changes, which can
then be applied with `plot:flush()` to refresh the window.

Multiple layers can be used effectively in animations. One layer can hold
static graphical elements, while another layer can be dedicated to dynamic
elements, creating a more complex animation.

Building
--------

The graphics module can be built using the Meson build system with the command:

```sh
# Use the lua option to choose between: lua5.1, lua5.4 and luajit
meson setup -Dlua=lua5.4 build
```

To build the following development packages are required:

- the freetype2 library
- the corresponding Lua or LuaJIT development package

The AGG library is used but it will be automatically downloaded and built so the corresponding
development package is not needed.

History
-------

The Lua Graphics Module was previously part of GSL Shell where it is still used. By looking at GSL Shell you can find more example of its utilisation.

Try It Out
----------

You can easily try the Lua Graphics Toolkit on Windows by downloading the binary packages
available in the github repository [release page](https://github.com/franko/graph-toolkit/releases).
The packages includes the graphics module and the Lua or LuaJIT executable itself.

