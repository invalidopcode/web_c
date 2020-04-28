# build instructions for web_c helpers

##### source file spec
SF_HELPER_C := $(shell find $(SDIR_HELPERS) -type f -name "*.c")
SF_HELPER := $(SF_HELPER_C)

##### intermediate file spec
IF_HELPER_WEB := $(patsubst $(SDIR_HELPERS)/%.c, \
	                          $(IDIR_WEB_OBJ)/%.bc, \
														$(SF_HELPER_C))
IF_HELPER_WEB_DEP := $(patsubst $(SDIR_HELPERS)/%.c, \
	                              $(IDIR_WEB_OBJ)/%.bc.d, \
																$(SF_HELPER_C))
IF_HELPER_CON := $(patsubst $(SDIR_HELPERS)/%.c, \
	                          $(IDIR_CON_OBJ)/%.$(EXT_CON_OBJ), \
														$(SF_HELPER_C))
IF_HELPER_CON_DEP := $(patsubst $(SDIR_HELPERS)/%.c, \
	                              $(IDIR_CON_OBJ)/%.$(EXT_CON_OBJ).d, \
																$(SF_HELPER_C))

IF_HELPER := $(IF_HELPER_WEB) $(IF_HELPER_CON) \
	           $(IF_HELPER_WEB_DEP) $(IF_HELPER_CON_DEP)

##### target file spec
TF_HELPER := 

# (everything is statically linked, so all helper output files are
# intermediaries & not needed after build)

