#####
##### Include config from other Makefiles
##### These are the ones you should be editing, NOT this file!
#####

##### Include basic config
include config-default.mk

##### Include any extra file generated by ./configure
-include configure.mk

##### Include Makefiles for helpers, elements, theme, and site
-include $(HELPERS_DIR)/helpers.mk
-include $(ELEMENTS_DIR)/elements.mk
-include $(THEMES_DIR)/theme.mk
-include $(SITE_DIR)/site.mk

#####
##### Scanning for files
#####

##### Elements
EL_C := $(shell find $(EL_SRC_DIR) -type f -name "*.c")
EL_BC := $(patsubst $(EL_SRC_DIR)/%.c,$(BC_DIR)/%.bc,$(EL_C))
#EL_BC := $(ELEMENTS)
EL_TARGETS := $(patsubst $(BC_DIR)/%.bc,$(SCRIPTS_DIR)/%.js,$(EL_BC))
EL_WASM := $(patsubst %.js,%.wasm,$(EL_TARGETS))

##### Helpers
HELPER_C := $(shell find $(HELPER_SRC_DIR) -type f -name "*.c")
HELPER_BC := $(patsubst $(HELPER_SRC_DIR)/%.c,$(BC_DIR)/%.bc,$(HELPER_C))
#HELPER_BC := $(HELPERS)

##### HTML Templates (need compiling)
THEME_TEMPLATES := $(shell find $(THEME_SRC_DIR) -type f -name "*.htmpl")
THEME_TEMPLATE_TARGETS := $(patsubst $(THEME_SRC_DIR)/%.htmpl,$(BUILD_DIR)/%.html,$(THEME_TEMPLATES))

SITE_TEMPLATES := $(shell find $(SITE_SRC_DIR) -type f -name "*.htmpl")
SITE_TEMPLATE_TARGETS := $(patsubst $(SITE_SRC_DIR)/%.htmlpl,$(BUILD_DIR)/%.html,$(SITE_TEMPLATES))

##### Static HTML
THEME_STATIC := $(shell find $(THEME_SRC_DIR) -type f -name "*.html")
THEME_STATIC_TARGETS := $(patsubst $(THEME_SRC_DIR)/%.html,$(BUILD_DIR)/%.html,$(THEME_STATIC))

SITE_STATIC := $(shell find $(SITE_SRC_DIR) -type f -name "*.html")
SITE_STATIC_TARGETS := $(patsubst $(SITE_SRC_DIR)/%.html,$(BUILD_DIR)/%.html,$(SITE_STATIC))

##### Misc
TEMPLATE_HTML_TARGETS := $(THEME_TEMPLATE_TARGETS) $(THEME_STATIC_TARGETS)
STATIC_HTML_TARGETS := $(SITE_TEMPLATE_TARGETS) $(SITE_STATIC_TARGETS)
HTML_TARGETS := $(TEMPLATE_HTML_TARGETS) $(STATIC_HTML_TARGETS)

#####
##### Main Rules 
#####

.PHONY: all clean dist_clean run

all: $(EL_TARGETS) $(HTML_TARGETS)

clean:
	-$(RM) $(wildcard $(HELPER_BC) $(EL_BC))

dist_clean: clean
	-$(RM) $(wildcard $(EL_TARGETS) $(EL_WASM) $(HTML_TARGETS))

##### run rule to simplify testing
run: all $(BUILD_DIR)/index.html
	cd $(BUILD_DIR) && emrun index.html

#####
##### Specific Rules
#####

#####
##### Generic Rules
#####

$(THEME_STATIC_TARGETS): $(BUILD_DIR)/%.html: $(THEME_SRC_DIR)/%.html
	$(CP) $< $@

$(SITE_STATIC_TARGETS): $(BUILD_DIR)/%.html: $(SITE_SRC_DIR)/%.html
	$(CP) $< $@

$(THEME_TEMPLATE_TARGETS): $(BUILD_DIR)/%.html: $(THEME_SRC_DIR)/%.htmpl
	$(HTMPL) $< $@

$(SITE_TEMPLATE_TARGETS): $(BUILD_DIR)/%.html: $(SITE_SRC_DIR)/%.htmpl
	$(HTMPL) $< $@

$(HELPER_BC): $(BC_DIR)/%.bc: $(HELPER_SRC_DIR)/%.c
	$(CC) $< $(CFLAGS_BC) -o $@

$(EL_BC): $(BC_DIR)/%.bc: $(EL_SRC_DIR)/%.c
	$(CC) $< $(CFLAGS_BC) -o $@

$(EL_TARGETS): $(SCRIPTS_DIR)/%.js: $(BC_DIR)/%.bc $(HELPER_BC)
	$(CC) $< $(HELPER_BC) $(CFLAGS_JS) -o $@

#TODO: dependency generation for c files

