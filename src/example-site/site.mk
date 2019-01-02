#####
##### site.mk - config & build rules for a web_c site
#####

##### json files, which we want copied to build root
SITE_JSON := $(shell find $(SITE_SRC_DIR) -type f -name "*.json")

##### Build list of targets, adding our custom ones on top of pre-existing
SITE_JSON_TARGETS := $(patsubst $(SITE_SRC_DIR)/%.json,$(BUILD_DIR)/%.json,$(SITE_JSON))
SITE_TARGETS := $(SITE_TARGETS) $(SITE_JSON_TARGETS)

##### Custom build rules
$(BUILD_DIR)/%.json: $(SITE_SRC_DIR)/%.json
	$(CP) $< $@
# copy json files to build dir

