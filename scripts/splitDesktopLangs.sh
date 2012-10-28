#!/bin/bash
#
# Splits a desktop file containing multiple languages
# e.g.:
#   razor-audio-video.directory
# into:
#   razor-audio-video.directory.in
#   translations/razor-audio-video_zh_TW.directory
#   translations/razor-audio-video_uk.directory
#   ...

TR_DIR=translations

split_file()
{
    local file="$1"
    echo "Splitting $file"

    (
        grep -v '\[\(.*\)\]\s*=' "${file}"
        echo
        echo "#TRANSLATIONS_DIR=${TR_DIR}"
        echo
        echo "# Translations"
    ) > "${file}.in"

    sed -n -e 's:.*\[\(.*\)\]\s*=.*:\1:p' "${file}" | while read lang; do
        out="${TR_DIR}/${file%.*}_${lang}.${file##*.}"
        (
            cat "${file}.in"
            grep "\[${lang}\]\s*="  "${file}"
        ) > "$out"
    done

    rm "${file}"
}

mkdir -p "${TR_DIR}"

for file in $@; do
    split_file $1
    shift;
done
