#!/bin/bash

PROJECT=qtxdg
export OPTS="-noobsolete"
export TARGET="-recursive .."

../../../scripts/translate-one.sh "$@"
