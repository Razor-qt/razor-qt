#!/bin/bash

COMMAND="$1"
INPUT="$2"
TYPE="$3"
OUTPUT="$4"

if [[ "x${TYPE}" == "xadaptor" ]] ; then
	TMP="$(mktemp)"
	"${COMMAND}" "${INPUT}" --"${TYPE}"="${TMP}"
	"$(dirname $0)/patch_adaptor.pl" < "${TMP}" > "${OUTPUT}"
else
	"${COMMAND}" "${INPUT}" --"${TYPE}"="${OUTPUT}"
fi
