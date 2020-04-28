#####
##### site.mk - site-wide definitions for comutiny-client
#####

TDIR_ROOT = $(DIR_SITE)/out
#  (keep intermediate and finished files seperate)

SITE_CONFIG_JSON := $(DIR_SITE)/site_config.json

SDIR_PAGES = $(SDIR_CONTENT)/pages
SDIR_POSTS = $(SDIR_CONTENT)/posts

IDIR_PAGES = $(IDIR_ROOT)/page_content
IDIR_POSTS = $(IDIR_ROOT)/post_content

TDIR_CONTENT_PAGES = $(IDIR_PAGES)
TDIR_CONTENT_POSTS = $(IDIR_POSTS)

TDIR_PAGES = $(TDIR_ROOT)
TDIR_POSTS = $(TDIR_ROOT)/posts

