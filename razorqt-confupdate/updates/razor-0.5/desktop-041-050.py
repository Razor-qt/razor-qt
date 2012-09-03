#!/usr/bin/env
#
# port 0.4.x desktop.conf to 0.5.0 format
#
# from:
#
#[razor]
#screens\1\desktops\1\plugins=iconview_default, analogclock_default
#screens\1\desktops\size=1
#screens\size=1
#screens\1\desktops\2\plugins=iconview_default12, analogclock_default12
#screens\2\desktops\1\plugins=iconview_default21, analogclock_default21
#
# to:
#
#[razor]
#plugins=iconview_default, analogclock_default, iconview_default12, analogclock_default12, iconview_default21, analogclock_default21
#screens\1\desktops\size=1
#screens\size=1
#
# rest of file lines are kept untouched


import string, sys


def getPlugins(s):
    parts = string.split(s, "=", 1)
    return parts[1]+", "


cont_old = sys.stdin.read()
cont_new = list()

inGroup = False;
plugins = ""

for i in cont_old.splitlines():
    if i == '[razor]':
        inGroup = True

    if inGroup and len(i) > 0 and i != '[razor]' and i[0] == '[':
        inGroup = False
        
    if inGroup:
        if string.find(i, 'plugins') != -1:
            plugins += getPlugins(i)
        else:
            cont_new.append(i);
    else:
        cont_new.append(i);
        

outstr = string.join(cont_new, "\n")
print string.replace(outstr, "[razor]", "[razor]\nplugins="+plugins[0:len(plugins)-2])
