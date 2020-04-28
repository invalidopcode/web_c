#####
##### site.mk - config & build rules for a web_c site
#####

##### source file spec
SF_CONTENT_DATA := $(shell find $(SDIR_CONTENT) -type f -name "*.json")
SF_CONTENT_HTML := $(shell find $(SDIR_CONTENT) -type f -name "*.html")
SF_CONTENT := $(SF_CONTENT_DATA) $(SF_CONTENT_HTML)

##### target file spec
TF_CONTENT_DATA := $(patsubst $(SDIR_CONTENT)/%.json, \
	                            $(TDIR_ROOT)/%.json, \
															$(SF_CONTENT_DATA))
TF_CONTENT_JSON := $(patsubst $(SDIR_CONTENT)/%.html, \
	                            $(TDIR_CONTENT)/%.json, \
															$(SF_CONTENT_HTML))
TF_CONTENT := $(TF_CONTENT_DATA) $(TF_CONTENT_JSON)

##### Custom build rules
$(TDIR_ROOT)/%.json: $(SDIR_CONTENT)/%.json
	$(MKDIR_P) $(@D)
	$(CP) $< $@
# copy json files to build dir

##### Build content json from html
$(TF_CONTENT_JSON): $(TDIR_CONTENT)/%.json \
	                : $(SDIR_CONTENT)/%.html \
									  $(TOOL_CONTENT2JSON)
	$(MKDIR_P) $(@D)
	$(TOOL_CONTENT2JSON) $< > $@


