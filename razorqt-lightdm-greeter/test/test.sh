#!/bin/sh
# Runs lightdm with razor-lightdm-greeter in Xephyr. You must have Xephyr installed to run this
lightdm --test-mode --debug --config=${CMAKE_CURRENT_BINARY_DIR}/test/lightdm.conf
