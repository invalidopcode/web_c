#####
##### theme.mk - targets and build rules for basic theme (test site)
#####

##### source file spec
SF_THEME_CSS := $(shell find $(SDIR_THEME) -type f -name "*.css")
SF_PAGE_TEMPLATE := $(SDIR_THEME)/page.mustache
SF_THEME := $(SF_THEME_CSS) $(SF_PAGE_TEMPLATE)

##### intermediate file spec 
IF_THEME := 

##### target file spec
TF_THEME_CSS := $(patsubst $(SDIR_THEME)/%.css, \
	                         $(TDIR_STYLE)/%.css, \
													 $(SF_THEME_CSS))
TF_THEME_HTML := $(patsubst $(TDIR_CONTENT)/%.json, \
	                          $(TDIR_PAGES)/%.html, \
														$(TF_CONTENT_JSON))
TF_THEME := $(TF_THEME_CSS) $(TF_THEME_HTML)


##### rule to generate html pages from content
$(TF_THEME_HTML): $(TDIR_PAGES)/%.html: $(TDIR_CONTENT)/%.json
	$(MKDIR_P) $(@D)
	$(MUSTACHE) $< $(SF_PAGE_TEMPLATE) > $@

##### rule to copy css to site
$(TF_THEME_CSS): $(TDIR_STYLE)/%.css: $(SDIR_THEME)/%.css
	$(MKDIR_P) $(@D)
	@$(CP) $< $@


