# build instructions for webc_tools

##### vars used in other make rules
TOOL_CONTENT2JSON := $(TDIR_TOOLS)/content2json

##### source file spec
SF_TOOLS := $(shell find $(SDIR_TOOLS) -type f -name "*.c")

##### intermediate file spec
IF_TOOLS_DEP := $(TOOL_CONTENT2JSON).d
IF_TOOLS := $(IF_TOOLS_DEP)
#  (this isn't strictly correct - tool deps should be target files, not
#   intermediate files. However, this way simplifies the main Makefile,
#   so it seems a fair trade-off)

##### target file spec
TF_TOOLS := $(TOOL_CONTENT2JSON)


##### build rules
$(TOOL_CONTENT2JSON): $(SDIR_TOOLS)/content2json.c \
	                    $(wildcard $(SDIR_HELPERS)/jcmap/*.c)
	$(MKDIR_P) $(@D)
	$(CC_CON) $< $(wildcard $(SDIR_HELPERS)/jcmap/*.c) -o $@ $(CFLAGS_TOOLS)

$(TDIR_TOOLS)/content2json.d: $(SDIR_TOOLS)/content2json.c
	$(MKDIR_P) $(@D)
	$(CC_CON) $< -M -MF $@ -MT $(TDIR_TOOLS)/content2json $(CFLAGS_TOOLS)

