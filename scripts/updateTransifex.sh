#!/bin/bash

PROJECT_ID=razor-qt

DIR=$1
if [ -z "$DIR" ]; then
    DIR='../'
fi

function searchConfig()
{
  local d=$1
  
  while [ "$d" != "/" ]; do
    if [ -f "$d/.tx/config" ]; then
      echo "$d/.tx/config"
      return 0
    fi
    
    d=`dirname $d`
  done

  return 1
}

function addTsFile()
{
  local SRC_FILE=$1
  local RESOURCE_ID=`basename ${SRC_FILE} .ts.src`
  local local TR_DIR=`dirname ${SRC_FILE}`

  local FIXED_SRC=`echo ${SRC_FILE} | sed -e"s|${PROJECT_BASE}/*||"`
  local FIXED_DIR=`echo ${TR_DIR}   | sed -e"s|${PROJECT_BASE}/*||"`
  local FILTER=${FIXED_DIR}/`basename ${SRC_FILE} .ts.src`'_<lang>.ts'
  
  echo "${PROJECT_ID}.${RESOURCE_ID}"

  echo "[${PROJECT_ID}.${RESOURCE_ID}]" >> ${TX_CONFIG}
  echo "type = QT"                      >> ${TX_CONFIG}
  echo "source_file = ${FIXED_SRC}"     >> ${TX_CONFIG}
  echo "source_lang = en"               >> ${TX_CONFIG}
  echo "file_filter = ${FILTER}"        >> ${TX_CONFIG}
  echo                                  >> ${TX_CONFIG}
}


function addDesktopFile
{
  local SRC_FILE=$1
  local TR_DIR=$2
  
  local SRC_DIR=`dirname ${SRC_FILE}`
  local RESOURCE_ID=`basename ${SRC_FILE} .desktop.in`_desktop
  local FIXED_SRC=`echo ${SRC_FILE} | sed -e"s|${PROJECT_BASE}/*||"`
  local FIXED_DIR=`readlink -m "${SRC_DIR}/${TR_DIR}" | sed -e"s|${PROJECT_BASE}/*||"`
  local FILTER=${FIXED_DIR}/`basename ${SRC_FILE} .desktop.in`'_<lang>.desktop'
  
  echo "${PROJECT_ID}.${RESOURCE_ID}"

  echo "[${PROJECT_ID}.${RESOURCE_ID}]" >> ${TX_CONFIG}
  echo "type = DESKTOP"                 >> ${TX_CONFIG}
  echo "source_file = ${FIXED_SRC}"     >> ${TX_CONFIG}
  echo "source_lang = en"               >> ${TX_CONFIG}
  echo "file_filter = ${FILTER}"        >> ${TX_CONFIG}
  echo                                  >> ${TX_CONFIG}

}



DIR=`readlink -m "$DIR"`
TX_CONFIG=`searchConfig $DIR` 
PROJECT_BASE=`dirname \`dirname "${TX_CONFIG}"\``


if [ -z "${TX_CONFIG}" ]; then
  echo "Not a git Transifex (or any of the parent directories): .tx" >&2
  exit 2
fi

echo -n                                  > ${TX_CONFIG}
echo "[main]"                           >> ${TX_CONFIG} 
echo "host = https://www.transifex.net" >> ${TX_CONFIG}
echo "type = QT"                        >> ${TX_CONFIG}
echo                                    >> ${TX_CONFIG}

for SRC_FILE in `find ${DIR} -name *.ts.src`; do
    addTsFile ${SRC_FILE}
done

for SRC_FILE in `find ${DIR} -name *.desktop.in`; do
  TR_DIR=`awk -F"=" '/#TRANSLATIONS_DIR=/ {print($2)}' ${SRC_FILE}`
  
  addDesktopFile ${SRC_FILE} ${TR_DIR}
done

