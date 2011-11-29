#!/bin/bash
#
# Example usage: test one plugin in the debugger:
# ./testPanelPlugin.sh clock gdb --args
#
# in valgrind:
#
# ./testPanelPlugin.sh clock valgrind --leak-check=full --show-reachable=yes
#
# etc.
#

PLUGIN=$1
shift
METHOD=$@
CFG_FILE=~/.razor/razor-panel/razor-panel-testplugin.conf

cat  > $CFG_FILE  <<END_OF_CONFIG
[panel]
plugins=$PLUGIN

[$PLUGIN]
type=$PLUGIN
alignment=Left
END_OF_CONFIG


${METHOD} razor-panel razor-panel-testplugin.conf

