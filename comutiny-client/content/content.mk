#####
##### target list and custom rules for comutiny site
#####
### comutiny uses TWO levels of templating. First on the html content, because
### they need to pull in url data. THEN again in theme.mk. So the process of
### building json content for theme.mk to use goes as follows:
###
### * convert markdown to html (store in intermediate dir)
###    = SF_CONTENT_PAGE_MD -> IF_CONTENT_PAGE_FROM_MD
###    = SF_CONTENT_POST_MD -> IF_CONTENT_POST_FROM_MD
###    (md -> thtml; is thtml so no conflict with files in step 3)
###
### * copy html (store in intermediate dir)
###    = SF_CONTENT_PAGE_HTML -> IF_CONTENT_PAGE_FROM_HTML
###    = SF_CONTENT_POST_HTML -> IF_CONTENT_POST_FROM_HTML
###    (html -> thtml; is theml so no conflict with files in step 3)
###
### * run mustache on all html to make intermediary html (intermediate dir)
###    = IF_CONTENT_PAGE_PRE_TEMPLATE -> IF_CONTENT_PAGE_TEMPLATED
###    = IF_CONTENT_POST_PRE_TEMPLATE -> IF_CONTENT_POST_TEMPLATED
###    (thtml -> html)
###
### * run content2json on html to make content json (content target dir)
###    = IF_CONTENT_PAGE_TEMPLATED -> TF_CONTENT_PAGE_JSON
###    = IF_CONTENT_POST_TEMPLATED -> TF_CONTENT_POST_JSON
##     (html -> json)
###
### These ..._JSON files are then used by theme.mk to build the final site

##### source file spec
SF_CONTENT_PAGE_MD := $(shell find $(SDIR_PAGES) -type f -name "*.md")
SF_CONTENT_PAGE_HTML := $(shell find $(SDIR_PAGES) -type f -name "*.html")
SF_CONTENT := $(SF_CONTENT_PAGE_MD) $(SF_CONTENT_PAGE_HTML)


##### intermediate file spec
IF_CONTENT_PAGE_FROM_MD := $(patsubst $(SDIR_PAGES)/%.md, \
	                                    $(IDIR_PAGES)/%.thtml, \
																			$(SF_CONTENT_PAGE_MD))
IF_CONTENT_PAGE_FROM_HTML := $(patsubst $(SDIR_PAGES)/%.html, \
	                                      $(IDIR_PAGES)/%.thtml, \
																				$(SF_CONTENT_PAGE_HTML))
IF_CONTENT_PAGE_PRE_TEMPLATE := $(IF_CONTENT_PAGE_FROM_MD) \
	                              $(IF_CONTENT_PAGE_FROM_HTML)
IF_CONTENT_PAGE := $(patsubst %.thtml, \
	                            %.html, \
															$(IF_CONTENT_PAGE_PRE_TEMPLATE))
IF_CONTENT := $(IF_CONTENT_PAGE) $(IF_CONTENT_PAGE_PRE_TEMPLATE)


##### target file spec
TF_CONTENT_PAGE_JSON := $(patsubst $(IDIR_PAGES)/%.html, \
	                                 $(TDIR_CONTENT_PAGES)/%.json, \
																	 $(IF_CONTENT_PAGE))
TF_CONTENT := $(TF_CONTENT_PAGE_JSON)


##### rules: convert markdown to html

##### rules: copy html
$(IF_CONTENT_PAGE_FROM_HTML): $(IDIR_PAGES)/%.thtml: $(SDIR_PAGES)/%.html
	$(MKDIR_P) $(@D)
	$(CP) $< $@ 

##### rules: run mustache on all html
$(IF_CONTENT_PAGE): %.html: %.thtml $(SITE_CONFIG_JSON)
	$(MKDIR_P) $(@D)
	$(MUSTACHE) $(SITE_CONFIG_JSON) $< > $@

##### rules: run content2json on all html
$(TF_CONTENT_PAGE_JSON): $(TDIR_CONTENT_PAGES)/%.json \
	                     : $(IDIR_PAGES)/%.html \
	                       $(SITE_CONFIG_JSON) \
											   $(TOOL_CONTENT2JSON)
	$(MKDIR_P) $(@D)
	$(TOOL_CONTENT2JSON) $< $(SITE_CONFIG_JSON) > $@

