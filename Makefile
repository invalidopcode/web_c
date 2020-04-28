#########################################
##### WEB_C PROJECT - base Makefile #####
#########################################
##### (See README.md for documentation, variable names, etc)

#####
##### Include config from other Makefiles
##### (These are the ones you should be editing, NOT this file!)
#####

##### Include basic default config
include config-default.mk

##### Include any extra file generated by ./configure
-include configure.mk

##### Include default site config (may need to override defaults, and respect
##### user-provided DIR_SITE. However, site.mk SHOULDN'T override
##### user-provided variables. Solve this using OVERRIDE_DEFAULT_... vars)
-include $(DIR_SITE)/site.mk

##### Include Makefiles for site, tools, helpers, elements, theme, and content
include $(SDIR_TOOLS)/tools.mk
include $(SDIR_HELPERS)/helpers.mk
-include $(SDIR_EL)/elements.mk
-include $(SDIR_CONTENT)/content.mk
-include $(SDIR_THEME)/theme.mk
#   (NOTE - order of these is VERY important)


#####
##### Scan for files, build target lists
#####

##### (tools, helpers, theme, and content target lists are handled by
#####  their respective .mk files)

##### Elements Target Lists (centralised because 1. it simplifies creating
#####    sites, since this part is complex. 2. it may need to be changed or
#####    improved, eg for a new emscripten version, so this avoids changing
#####    every single site individually

ifndef EL_TARGETS_OVERRIDE
SF_EL_C := $(shell find $(SDIR_EL) -type f -name "*.c")
SF_EL := $(SF_EL_C)

IF_EL_WEB := $(patsubst $(SDIR_EL)/%.c, \
	                          $(IDIR_WEB_OBJ)/%.bc, \
														$(SF_EL_C))
IF_EL_WEB_DEP := $(patsubst $(SDIR_EL)/%.c, \
	                              $(IDIR_WEB_OBJ)/%.bc.d, \
																$(SF_EL_C))
IF_EL_CON := $(patsubst $(SDIR_EL)/%.c, \
	                          $(IDIR_CON_OBJ)/%.$(EXT_CON_OBJ), \
														$(SF_EL_C))
IF_EL_CON_DEP := $(patsubst $(SDIR_EL)/%.c, \
	                              $(IDIR_CON_OBJ)/%.$(EXT_CON_OBJ).d, \
																$(SF_EL_C))
IF_EL := $(IF_EL_WEB) $(IF_EL_CON) \
	       $(IF_EL_WEB_DEP) $(IF_EL_CON_DEP)


TF_EL_JS := $(patsubst $(IDIR_WEB_OBJ)/%.bc, \
	                     $(TDIR_SCRIPTS)/%.js, \
											 $(IF_EL_WEB))
TF_EL_WASM := $(patsubst %.js, %.wasm, $(TF_EL_JS))
TF_EL_BIN :=  $(patsubst $(IDIR_CON_OBJ)/%.$(EXT_CON_OBJ), \
	                       $(TDIR_CON_BIN)/%.$(EXT_CON_BIN), \
												 $(IF_EL_CON))
TF_EL := $(TF_EL_JS) $(TF_EL_WASM) $(TF_EL_BIN)
endif


##### General Target Lists

IF_DEP := $(IF_EL_WEB_DEP) $(IF_EL_CON_DEP) \
	        $(IF_HELPER_WEB_DEP) $(IF_HELPER_CON_DEP) \
					$(IF_TOOLS_DEP)



#####
##### Main Rules 
#####

.PHONY: all clean clean_site run list_vars
.SUFFIXES:
#  (blocks implicit rules - they're not needed)
.DEFAULT_GOAL:= all

all: $(TF_TOOLS) $(TF_EL) $(TF_HELPER) $(TF_THEME) $(TF_CONTENT) $(IF_DEP)

clean:
	-$(RM) $(wildcard $(IF_TOOLS) $(IF_HELPER) $(IF_CONTENT) $(IF_EL) $(IF_THEME)\
	                  $(TF_TOOLS) $(TF_HELPER) $(TF_CONTENT) $(TF_EL) $(TF_THEME))
	-$(RM) $(patsubst %.wasm, %.wast, $(TF_EL_WASM))
#    (ensure .wast files are cleaned - if this errors, no worries)

clean_site:
	-$(RM) $(wildcard $(IF_CONTENT) $(IF_EL) $(IF_THEME)\
	                  $(TF_CONTENT) $(TF_EL) $(TF_THEME))

##### run rule to simplify testing
run: all $(TDIR_ROOT)/index.html
	cd $(TDIR_ROOT) && emrun index.html


##### list_vars to help debugging
list_vars:
	@echo $(foreach V, \
		              $(sort $(.VARIABLES)), \
									$(if $(filter file,$(origin $V)), $(shell echo $V))) \
		| xargs -n1 echo


##### Pull in depfiles

IF_DEP_EXISTING := $(wildcard $(IF_DEP))
-include $(IF_DEP_EXISTING)



#####
##### Generic Rules
#####

##### Webassembly - objects, depfiles, and "binaries"

$(IF_HELPER_WEB): $(IDIR_WEB_OBJ)/%.bc: $(SDIR_HELPERS)/%.c
	$(MKDIR_P) $(@D)
	$(CC_WEB) $< -o $@ $(CFLAGS_WEB_OBJ) 

$(IF_HELPER_WEB_DEP): $(IDIR_WEB_OBJ)/%.bc.d: $(SDIR_HELPERS)/%.c
	$(CC_WEB) $< -M -MF $@ -MT $(IDIR_WEB_OBJ)/$*.bc $(CFLAGS_WEB_OBJ)

ifndef EL_RULES_OVERRIDE
$(IF_EL_WEB): $(IDIR_WEB_OBJ)/%.bc: $(SDIR_EL)/%.c
	$(MKDIR_P) $(@D)
	$(CC_WEB) $< -o $@ $(CFLAGS_WEB_OBJ)

$(IF_EL_WEB_DEP): $(IDIR_WEB_OBJ)/%.bc.d: $(SDIR_EL)/%.c
	$(CC_WEB) $< -M -MF $@ -MT $(IDIR_WEB_OBJ)/$*.bc $(CFLAGS_WEB_OBJ)

$(TF_EL_JS): $(TDIR_SCRIPTS)/%.js: $(IDIR_WEB_OBJ)/%.bc $(IF_HELPER_WEB)
	$(MKDIR_P) $(@D)
	$(CC_WEB) $< $(IF_HELPER_WEB) -o $@ -s EXPORT_NAME=$(*F) $(CFLAGS_JS)
endif


##### Console - objects, depfiles, and "binaries"

$(IF_HELPER_CON): $(IDIR_CON_OBJ)/%.$(EXT_CON_OBJ): $(SDIR_HELPERS)/%.c
	$(MKDIR_P) $(@D)
	$(CC_CON) $< -o $@ $(CFLAGS_CON_OBJ)

$(IF_HELPER_CON_DEP): $(IDIR_CON_OBJ)/%.$(EXT_CON_OBJ).d: $(SDIR_HELPERS)/%.c
	$(CC_WEB) $< -M -MF $@ -MT $(IDIR_CON_OBJ)/$*.$(EXT_CON_OBJ) \
		        $(CFLAGS_CON_OBJ)

ifndef EL_RULES_OVERRIDE
$(IF_EL_CON): $(IDIR_CON_OBJ)/%.$(EXT_CON_OBJ): $(SDIR_EL)/%.c
	$(MKDIR_P) $(@D)
	$(CC_CON) $< -o $@ $(CFLAGS_CON_OBJ)

$(IF_EL_CON_DEP): $(IDIR_CON_OBJ)/%.$(EXT_CON_OBJ).d: $(SDIR_EL)/%.c
	$(CC_WEB) $< -M -MF $@ -MT $(IDIR_CON_OBJ)/$*.$(EXT_CON_OBJ) \
		        $(CFLAGS_CON_OBJ)

$(TF_EL_BIN): $(TDIR_CON_BIN)/%.$(EXT_CON_BIN) \
	          : $(IDIR_CON_OBJ)/%.$(EXT_CON_OBJ) \
	            $(IF_HELPER_CON)
	$(MKDIR_P) $(@D)
	$(CC_CON) $< $(IF_HELPER_CON) -o $@ $(CFLAGS_CON_BIN)
endif

##### (why centralise this? As well as the reasons given for centralising
#####  element target lists, the building of helpers and elements is tightly
#####  linked, so this seems more elegant)

