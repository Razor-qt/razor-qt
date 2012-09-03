#!/usr/bin/env python
"""
Part of the Razor-Qt project

Port desktop.conf syntax from 0.4.x to 0.5.0 format

OLD:
[razor]
screens\1\desktops\1\plugins=iconview_default, analogclock_default
screens\1\desktops\size=1
screens\size=1
screens\1\desktops\2\plugins=iconview_default12, analogclock_default12
screens\2\desktops\1\plugins=iconview_default21, analogclock_default21

NEW:
[razor]
plugins=iconview_default, analogclock_default, iconview_default12, analogclock_default12, iconview_default21, analogclock_default21
screens\1\desktops\size=1
screens\size=1
"""

import re
import sys
try:
	from configparser import ConfigParser
except ImportError:
	from ConfigParser import ConfigParser

QT_ARRAY_REGEX = re.compile(r"screens\\(\d+)\\desktops\\(\d+)\\plugins")

def main():
	cfg = ConfigParser()
	cfg.readfp(sys.stdin)

	for section in cfg.sections():
		if section != "razor":
			# Only touch the [razor] section
			continue

		for key, value in cfg.items(section):
			if QT_ARRAY_REGEX.match(key):
				plugins = cfg.get(section, key).split(",")

				if not cfg.has_option(section, "plugins"):
					cfg.set(section, "plugins", "")

				plugins = cfg.get(section, "plugins").split(",") + plugins

				cfg.set(section, "plugins", ", ".join(x.strip() for x in plugins if x.strip()))
				cfg.remove_option(section, key)

	cfg.write(sys.stdout)


if __name__ == "__main__":
	main()
