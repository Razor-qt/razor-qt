#!/bin/sh

export PROJECT=razor-session
export OPTS="-noobsolete"
export TARGET="-recursive .."

../../scripts/translate-one.sh "$@"
