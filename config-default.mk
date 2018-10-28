##### Default configuration options for web_c build
#####
##### Please add to or override these in the configure.mk, elements.mk,
##### helpers.mk, theme.mk, and site.mk files.

##### Theme Configuration. Set only name here, directory set later
THEME := example

##### Directories Configuration
BUILD_DIR := build
BC_DIR := $(BUILD_DIR)/bc
SCRIPTS_DIR := $(BUILD_DIR)/scripts
CSS_DIR := $(BUILD_DIR)/css

SRC_DIR := src
EL_SRC_DIR := $(SRC_DIR)/elements
HELPER_SRC_DIR := $(SRC_DIR)/el_helpers
THEMES_SRC_DIR := $(SRC_DIR)/themes
THEME_SRC_DIR := $(THEMES_SRC_DIR)/$(THEME)
SITE_SRC_DIR := $(SRC_DIR)/example-site

##### Flags configuration - include environment CFLAGS too
CFLAGS_BC := $(CFLAGS) -c -I$(HELPER_SRC_DIR)
CFLAGS_JS := $(CFLAGS) -s WASM=1 -s ENVIRONMENT=web -s MODULARIZE_INSTANCE=1 -s FETCH=1 -I$(HELPER_SRC_DIR)
CFLAGS_DEP := -M $(CFLAGS_BC)

## Flag reasons:
# -M - create a dependency file INSTEAD of normal compiling
# -c - only do bytecode compilation
# -I... - add helper includes to search path
# -s FETCH=1 - link to fetch api
# -s MODULARIZE_INSTANCE=1 - complicated reasons - tldr: it's simpler
# -s ENVIRONMENT=web - TODO
# -s WASM=1 - TODO

##### Binaries Configuration
CC := emcc
GCC := gcc
SASS := sass
RM := rm
CP := cp
CD := cd
HTMPL := cp
SED := sed

