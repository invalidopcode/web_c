#!/bin/bash
#
# usage: mkpage.sh <page_file> <page_template> <config_template> <target>
# WARNING makes temp files in working directory
set -o errexit -o pipefail -o nounset

PAGE_FILE=$1
PAGE_TEMPLATE=$2
CONFIG_TEMPLATE=$3
TMP_TARGET=$(mktemp ./XXXXXX.html)
TMP_TARGET_2=$(mktemp ./XXXXXX.html)
TARGET=$4

TMPJSON_1=$(mktemp ./XXXXXX.json)
TMPJSON_2=$(mktemp ./XXXXXX.json)

# cleanup function for temp files
mkpage_cleanup() {
  rm $TMPJSON_1
  rm $TMPJSON_2
  rm $TMP_TARGET
  rm $TMP_TARGET_2
  exit 1
}

# check arguments
if [ $# -ne 4 ]; then
  echo "usage: mkpage.sh <page_file> <page_template> <config_template> <target>"
  mkpage_cleanup
fi

# does page have custom json for us to splice into config?
if grep -q "<!--@COMUTINY_META>" $PAGE_FILE; then
  # extract custom json from page
  sed -n "/<!--@COMUTINY_META>/,/<@-->/p" $PAGE_FILE | sed -e "s/<!--@COMUTINY_META>//g" | sed -e "s/<@-->//g" > $TMPJSON_1 || mkpage_cleanup
  
  # build temp config file
  cat $CONFIG_TEMPLATE | sed -e "/\"@custom>\"/{r $TMPJSON_1" -e 'd' -e '}' > $TMPJSON_2 || mkpage_cleanup
else
  # next part expects config in temp file either way...
  cp $CONFIG_TEMPLATE $TMPJSON_2
fi

# apply to template - 1st via mustache, then sub-ing in our content w/sed
cat $PAGE_TEMPLATE | sed -e "/<!--@content>-->/{r $PAGE_FILE" -e 'd' -e '}' >$TMP_TARGET || mkpage_cleanup
mustach $TMPJSON_2 $TMP_TARGET >$TMP_TARGET_2 || mkpage_cleanup

# copy to target last - avoids writing data on fail
rm $TMP_TARGET
mv $TMP_TARGET_2 $TARGET
rm $TMPJSON_1
rm $TMPJSON_2

