# web\_c

A starting point for c-based web apps. Created as an experiment, will see how far it can go!

web\_c uses a client-server model. Server side code uses the Kore Engine, it is
intended that server functions are split into microservers each built and
started using the Kore tools.

Client side code provides dynamic content using *elements* written in c and
compiled to javascript using emscripten. The pages etc of a site are built
using Makefiles which generate content and apply a theme.

Note that web\_c is a long way from production-ready, and much of the code base
is still essentially made up of hacks.

[[toc]]

# Client Side

## Parts

Each of these are contained in their own directory and may have their own
Makefile

* *tools* `src/tools` are utilities used in the build process
* *helpers* `src/helpers` provide functions used by _elements_ - for example
writing data to a page, processing json. Also used by _tools_.
* *elements* `\<site\>/elements` compile into scripts that can be included by
html pages. Each element targets a specific class name and will render content
to html elements using that name. *Experimentally*, will also  be compiled
into binaries for use in automated tests, and fallback server-side logic
* *content* `\<site\>/content` the main html of the client side site. Can
either be normal static html files, or be programatically generated using the
content.mk Makefile. In the example sites, content.mk transforms the html
content and metadata into json files, to be read by theme.mk
* *theme* `\<site\>/theme` the theme takes the content and transforms it
into a built site, properly stylised

## Example Sites

* *test* runs a series of tests on the code in src/helpers/jcmap and displays
the results
* *comutiny-client* is a work-in-progress lightweight blog

Both example sites can be built by setting the SITE and THEME variables in
config-default.mk, and running `make`. Run `make run` to test on the local
machine.

## Build Dependencies

* emscripten - for compilation to js https://kripken.github.io/emscripten-site/
* c compiler compatible with basic gcc options (tested on clang) - for
 compilation of tools, etc
* gnu make - for building
* normal posix utilities - find, rm, cp, etc
* mustache - used for building site from templates. Recommended: [https://gitlab.com/jobol/mustach]

All code expects to run in the C locale. Execution in other locales will cause
undefined behavoir.

In some systems (ie not GNU Core Utilities (‘fileutils’ version 4.1),
FreeBSD 5.0, NetBSD 2.0.2, or OpenBSD 2.4), mkdir -p has a race condition when
running in parallel. Do not run a parallel build on such systems.


## Building

TODO: proper configure script. For the meantime:

* edit configuration values as necessary in config-default.mk
* ensure emscripten environment set up correctly (normally `. emsdk_env.sh` from
the emdsk source directory is sufficient)
* run gnu make to build
* run `make run` to use emscripten's emrun tool to test the site. Will host
 build/index.html on localhost

`make all` will build the site into the target directory. This directory can be
served from a web server as-is.

## Tools

### content2json

Transforms a html file into a json string, used by theme Makefile to feed
content html into a Markdown template. Expects utf8 input, returns error on
non-utf8 characters.

JSON values for use by the template can be fed in through a file on the
command line (expected use: site-wide configuration), and through the
content file itself. This is done by writing a json object between two tags:
`<!--@WEBC_META\> ... <\-->`

All json is validated, and an error returned on invalid json in either the
site file or webc\_meta.

Finally, a "content\_type" can be specified for use by the theme, using
the tag:
`<!--@WEBC_TYPE="..."\-->`

This MUST be a valid json string and there MUST be no whitespace before the
'=' character. The string is validated and an error returned if invalid.

None of these values are required. The json objects default to an empty `{}`,
while the content\_type defaults to `page`.

Takes: \<content file\> [site json file]
Returns: json to stdout in the following form...
`{
  site: {<json from site json file>},
  page: {<json from page webc_meta tag>},
  content_type: "<json string from page webc_type tag>",
  content: "<escaped string version of html content>"
}`

This tool is still somewhat of a hack. The ideal would be to make it more
fault-tolerant, and for it to extract content from the body of a valid html
file.

## Makefile Structure

### Makefiles

* *config-default.mk* - default directories, configuration options, build tools
* *config.mk* - generated dynamically by `./configure`
* *Makefile* - base Makefile, includes all others and ties everything together.
Handles elements' targets and build rules, and helpers' build rules
* *src/tools/tools.mk* - target lists and build rules for tools
* *src/helpers/helpers.mk* - target lists for helpers
* *<site>/elements/elements.mk* - (optional) target lists and build rules for
elements. MAY set `IF_EL_TARGETS_OVERRIDE` or `IF_EL_RULES_OVERRIDE`, to
override default build rules and targets in base Makefile with its own. If it
does, then it SHOULD set `TF_EL` variable
* *<site>/content/content.mk* - creates the structure and raw content of the
site, outputting json files to `$(TDIR_CONTENT)/`. SHOULD set
`TF_CONTENT` variable
* *<site>/theme/theme.mk* - Transforms `$(TF_CONTENT)` files into a list of
target html pages and gives rules to create final site. SHOULD set `TF_THEME`
* *<site>/site.mk* - (optional) extra rules and definitions for site

### Variables

we use the following prefixes to make things clearer:

* *SF\_* - source file(s)
* *SDIR\_* - source directory
* *IF\_* - intermediate file(s)
* *IDIR\_* - intermediate directory
* *TF\_* - target file(s)
* *TDIR\_* - target directory
* *DIR\_* - other directory
* *EXT\_* - file extension/type

