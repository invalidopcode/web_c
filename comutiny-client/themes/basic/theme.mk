#####
##### theme.mk - targets and build rules for basic theme (comutiny-client)
#####

##### source file spec
SF_THEME_CSS := $(shell find $(SDIR_THEME) -type f -name "*.css")
SF_PAGE_TEMPLATE := $(SDIR_THEME)/page.mustache
#SF_POST_TEMPLATE := $(SDIR_THEME)/post.mustache

SF_THEME := $(SF_THEME_CSS) $(SF_PAGE_TEMPLATE) $(SF_POST_TEMPLATE)


##### intermediate file spec
IF_THEME := 


##### target file spec
TF_THEME_CSS := $(patsubst $(SDIR_THEME)/%.css, \
	                         $(TDIR_STYLE)/%.css, \
													 $(SF_THEME_CSS))
#TF_THEME_POST_HTML := $(patsubst $(TDIR_CONTENT_POSTS)/%.json, \
#	                                $(TDIR_POSTS)/%.html, \
#																	$(TF_CONTENT_POST_JSON))
TF_THEME_PAGE_HTML := $(patsubst $(TDIR_CONTENT_PAGES)/%.json, \
	                               $(TDIR_PAGES)/%.html, \
																 $(TF_CONTENT_PAGE_JSON))
TF_THEME := $(TF_THEME_CSS) $(TF_THEME_POST_HTML) $(TF_THEME_PAGE_HTML)


##### rules to generate html pages
#$(TF_THEME_POST_HTML): $(TDIR_POSTS)/%.html: $(TDIR_CONTENT_POSTS)/%.json \
#                                             $(SF_POST_TEMPLATE)
#	$(MKDIR_P) $(@D)
#	$(MUSTACHE) $< $(SF_POST_TEMPLATE) > $@

$(TF_THEME_PAGE_HTML): $(TDIR_PAGES)/%.html: $(TDIR_CONTENT_PAGES)/%.json \
	                                           $(SF_PAGE_TEMPLATE)
	$(MKDIR_P) $(@D)
	$(MUSTACHE) $< $(SF_PAGE_TEMPLATE) > $@

##### rule to copy css to site
$(TF_THEME_CSS): $(TDIR_STYLE)/%.css: $(SDIR_THEME)/%.css
	$(MKDIR_P) $(@D)
	@$(CP) $< $@

