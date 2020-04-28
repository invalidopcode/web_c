#!/bin/bash
#
# usage: mkpage.sh <site config> <raw page content> <target page>
# WARNING makes temp files in working directory
set -o errexit -o pipefail -o nounset

SITE_CONFIG=$1
RAW_PAGE=$2
TARGET=$3

TMP_TARGET=$(mktemp ./XXXXXX.html)
TMPJSON_1=$(mktemp ./XXXXXX.json)
TMPJSON_2=$(mktemp ./XXXXXX.json)

# cleanup function for temp files
mkpage_cleanup() {
  rm $TMP_TARGET
  rm $TMPJSON_1
  rm $TMPJSON_2
  exit 1
}

# check arguments
if [ $# -ne 3 ]; then
  echo "usage: mkpage.sh <site config> <raw page content> <target>"
  mkpage_cleanup
fi

# does page have custom json for us to splice into config?
if grep -q "<!--@COMUTINY_META>" $RAW_PAGE; then
  # extract custom json from page
  sed -n "/<!--@COMUTINY_META>/,/<@-->/p" $RAW_PAGE | sed -e "s/<!--@COMUTINY_META>//g" | sed -e "s/<@-->//g" > $TMPJSON_1 || mkpage_cleanup
  
  # build temp config file
  cat $SITE_CONFIG | sed -e "/\"@custom>\"/{r $TMPJSON_1" -e 'd' -e '}' > $TMPJSON_2 || mkpage_cleanup
else
  # next part expects config in temp file either way...
  cp $CONFIG_TEMPLATE $TMPJSON_2
fi

# find mustache magic and apply template (if exists)


# do our own replacements over the top


