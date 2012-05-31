#!/bin/bash
#
# Based on the QRegExp-Editor script:
# https://gitorious.org/qregexp-editor/qregexp-editor/blobs/master/scripts/astyle-qregexp_editor
#
# apply razor-qt coding style to all c, cpp and header files in and below the current directory
#
# the coding style is defined in https://github.com/Razor-qt/razor-qt/wiki/Coding-Style
#
# requirements: installed astyle => 1.24

OPTS="--indent=spaces=4 --style=allman --add-brackets \
      --indent-labels --pad-header --pad-oper --unpad-paren \
      --keep-one-line-blocks --convert-tabs \
      --indent-preprocessor  --align-pointer=name"

astyle ${OPTS} $(find . -regex ".*\.\(c\|cpp\|h\)$")
