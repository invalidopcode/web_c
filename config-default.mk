##### Default configuration options for web_c build
#####
##### Please add to or override these in the configure.mk, elements.mk,
##### helpers.mk, theme.mk, and site.mk files.

##### Main configuration
ENVIRONMENT := test
 #ENVIRONTMENT := production

 #SITE := test
 #THEME := example
SITE := comutiny-client
THEME := basic

##### Directories Configuration
BUILD_DIR := $(SITE)/build
BC_DIR := $(BUILD_DIR)/bc
SCRIPTS_DIR := $(BUILD_DIR)/scripts
STYLE_DIR := $(BUILD_DIR)/css

SRC_DIR := src
HELPER_SRC_DIR := $(SRC_DIR)/el_helpers

SITE_DIR := $(SITE)
EL_SRC_DIR := $(SITE_DIR)/elements
THEMES_SRC_DIR := $(SITE_DIR)/themes
THEME_SRC_DIR := $(THEMES_SRC_DIR)/$(THEME)
SITE_SRC_DIR := $(SITE_DIR)/site

##### Flags configuration - include environment CFLAGS too
CFLAGS_RUNTIME := -s "EXTRA_EXPORTED_RUNTIME_METHODS=['ALLOC_NORMAL', 'ALLOC_STACK', 'ALLOC_STATIC', 'ALLOC_DYNAMIC', 'ALLOC_NONE', 'print', 'printErr']"
ifeq ($(ENVIRONMENT),test)
	CFLAGS_BC := $(CFLAGS) -c -I$(HELPER_SRC_DIR) -DJCMAP_DEBUG
	CFLAGS_JS := $(CFLAGS) -s WASM=1 -s ENVIRONMENT=web -s MODULARIZE_INSTANCE=1 -s FETCH=1 -I$(HELPER_SRC_DIR) $(CFLAGS_RUNTIME) -O0
else
	CFLAGS_BC := $(CFLAGS) -c -I$(HELPER_SRC_DIR)
	CFLAGS_JS := $(CFLAGS) -s WASM=1 -s ENVIRONMENT=web -s MODULARIZE_INSTANCE=1 -s FETCH=1 -I$(HELPER_SRC_DIR) $(CFLAGS_RUNTIME) -O2
endif

CFLAGS_DEP := -M $(CFLAGS_BC)

## Flag reasons:
# -M - create a dependency file INSTEAD of normal compiling
# -c - only do bytecode compilation
# -I... - add helper includes to search path
# -s FETCH=1 - link to fetch api
# -s MODULARIZE_INSTANCE=1 - complicated reasons, tldr: it's simpler
# -s ENVIRONMENT=web - *probably* strips out code for handling non-web
#                     environment
# -s WASM=1 - we want wasm output, not just asm.js
# -s EXTRA_EXPORTED_RUNTIME... - couldn't get it to work without :/
# -O0 - compile faster (test environment)
# -02 - run faster (production environment)
# -DJCMAP_DEBUG - set constant so that debug messages printed by jcmap

##### Binaries Configuration
CC ?= clang
CC_WEB ?= emcc
CC_BIN := $(CC)
RM := rm
CP := cp
CD := cd
SED := sed
MKDIR := mkdir -p

