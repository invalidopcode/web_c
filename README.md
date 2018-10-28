# web_c
a starting point for c-based web apps

## Concepts

* *elements* are written in c and compiled to webassembly. When an element's
script is included in a html document, it will render content inside elements
with the corresponding class name
* *helpers* are written in c, compiled to object format (LLVM bytecode in this
case), and linked into each element webassembly script that is built. They
provide useful functions to element scripts (eg ability to access the DOM)
* *site* the main static html of the site in question
* *theme* used by the site to change appearance - themes can be switched
between at build time
* *static html* is all that's needed to make a fully functioning web app.
The build process copies all html to the build directory, preserving directory
structure
* *templated html* is html run through a simple templating engine before
being copied to the build directory as with static html

## Building

There'll be a proper configure script soon, but for the meantime:

# edit configuration values as necessary in config-default.mk
# run gnu make to build
# run `make run` to use emscripten's emrun tool to test the site. Will host
 build/index.html on localhost

`make all` will build the site into the build directory. This directory can be
served from any web server as-is.

