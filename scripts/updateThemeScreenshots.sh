#!/bin/bash

PROJECT_ID=razor-qt
TMP_DIR=/tmp

DIR=$1
if [ -z "$DIR" ]; then
    DIR='../'
fi

DIR=`readlink -m "$DIR"`


function prepareConfig()
{
    local THEME=$1
    mkdir -p $TMP_DIR/razor/razor-panel
    cat > $TMP_DIR/razor/razor-panel/screenshot.conf << PANEL_COFIG    
[panel]
width=100
width-percent=true
height=32
alignment=0
theme-size=false
desktop=0
position=Bottom
plugins=mainmenu, clock
plugins=mainmenu

[mainmenu]
alignment=Left
type=mainmenu

[clock]
alignment=Left
type=clock
PANEL_COFIG

    echo "[General]" 	   > $TMP_DIR/razor/razor.conf
    echo "theme=${THEME}" >> $TMP_DIR/razor/razor.conf
}

function makeScreenshot()
{
    local PID=$1
    local OUT_FILE=$2
    local WIN_ID=`wmctrl -l -p | grep ${PID} | awk '{print $1}'`
    if [ -z "$WIN_ID" ]; then
	echo "Can't find window ID for ${PID}"
	return
    fi
    import -window ${WIN_ID} ${TMP_DIR}/${PID}.png
    convert -crop 100x200+0+0 ${TMP_DIR}/${PID}.png ${OUT_FILE}

}


for THEME_DIR in `ls -d ${DIR}/razorqt-resources/themes/*/`; do
    echo $THEME_DIR
    prepareConfig `basename $THEME_DIR`
    LANG=C XDG_CONFIG_HOME=${TMP_DIR} razor-panel screenshot >/dev/null 2>&1 & PANEL_PID=$!
    sleep 1
    makeScreenshot $PANEL_PID $THEME_DIR/preview.png

    kill $PANEL_PID
done

