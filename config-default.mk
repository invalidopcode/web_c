##### Default configuration options for web_c build
#####
##### Please add to or override these in the configure.mk, elements.mk,
##### helpers.mk, theme.mk, and site.mk files.

##### Main configuration
ENVIRONMENT := test
 #ENVIRONTMENT := production

SITE := test
THEME := example
#SITE := comutiny-client
#THEME := basic

##### Base Directories Configuration
SDIR_ENGINE := src
DIR_SITE = $(SITE)
DIR_THEMES = $(DIR_SITE)/themes

IDIR_ROOT = $(DIR_SITE)/build
TDIR_ROOT = $(DIR_SITE)/build

##### Source Directories Configuration
SDIR_HELPERS = $(SDIR_ENGINE)/helpers
SDIR_TOOLS = $(SDIR_ENGINE)/tools
SDIR_EL = $(DIR_SITE)/elements
SDIR_THEME = $(DIR_THEMES)/$(THEME)
SDIR_CONTENT = $(DIR_SITE)/content

##### Intermediate Directories Configuration #####
IDIR_TOOLS = $(SDIR_TOOLS)

IDIR_WEB_OBJ = $(IDIR_ROOT)/bc
IDIR_CON_OBJ = $(IDIR_ROOT)/stdio

TDIR_CONTENT = $(IDIR_ROOT)/content

##### Target & Intermediate Directories Configuration
TDIR_TOOLS = bin

TDIR_CON_BIN = $(TDIR_ROOT)/stdio
TDIR_SCRIPTS = $(TDIR_ROOT)/scripts

TDIR_STYLE = $(TDIR_ROOT)/css
TDIR_PAGES = $(TDIR_ROOT)

##### Target Type Configuration
EXT_CON_OBJ := o
EXT_CON_BIN := elf
#    (used so we can switch to build bytecode later)

##### Flags configuration - include environment CFLAGS too
CFLAGS ?= -g
CFLAGS_RUNTIME := -s "EXTRA_EXPORTED_RUNTIME_METHODS=['ALLOC_NORMAL', \
	                                                    'ALLOC_STACK', \
																											'ALLOC_STATIC', \
																											'ALLOC_DYNAMIC', \
																											'ALLOC_NONE', \
																											'print', \
																											'printErr']"
CFLAGS_COMMON = -I$(SDIR_HELPERS) $(CFLAGS)

ifeq ($(ENVIRONMENT),test)
	CFLAGS_WEB_OBJ = -c -DJCMAP_DEBUG $(CFLAGS_COMMON)
	CFLAGS_JS = -s WASM=1 -s ENVIRONMENT=web \
				  		-s MODULARIZE_INSTANCE=1 -s FETCH=1 \
				  		$(CFLAGS_RUNTIME) -O0 $(CFLAGS_COMMON)
	CFLAGS_CON_OBJ = -c -DWEBC_STDIO $(CFLAGS_COMMON)
	CFLAGS_CON_BIN = -O0 -DWEBC_STDIO $(CFLAGS_COMMON)
else
	CFLAGS_WEB_OBJ = -c  $(CFLAGS_COMMON)
	CFLAGS_JS = -s WASM=1 -s ENVIRONMENT=web \
							-s MODULARIZE_INSTANCE=1 -s FETCH=1 \
							$(CFLAGS_RUNTIME) -O2 $(CFLAGS_COMMON)
	CFLAGS_CON_OBJ = -c -DWEBC_STDIO $(CFLAGS_COMMON)
	CFLAGS_CON_BIN = -O2 -DWEBC_STDIO $(CFLAGS_COMMON)
endif

#CFLAGS_TOOLS := -Wall -Werror -DWEBC_STDIO $(CFLAGS_COMMON) 
CFLAGS_TOOLS = -DWEBC_STDIO $(CFLAGS_COMMON)

CFLAGS_DEP = -M $(CFLAGS_WEB_OBJ)

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
# -DWEBC_STDIO - disable emscripten in helpers
# -Wall - enable all warnings
# -Werror - warnings are errors - ensures good code quality

##### Binaries Configuration
CC ?= clang
CC_WEB ?= emcc
CC_CON := $(CC)
RM := rm
CP := cp
MKDIR_P := mkdir -p
MUSTACHE := mustach
SHELL = /bin/sh

