#!/bin/bash

function help {
  echo "Usage: makeDeb.sh [otions] <path-to-source>"
  echo
  echo "Options"
  echo "  -h|--help             display this message"
  echo "  -o|--outdirt=DIR      write result to DIR"
  echo "  -d|--dist=DIST        buld for distributive ubuntu/debian"
  echo "  -r|--release=RELEASE  release name (sid, maveric, natty etc.)"
  echo "  -ver=VERSION          razor version"
  echo "  -b|--binary           build a binary package, if ommited build only only a source package"
}

NAME='razorqt'
TYPE='-S'
OUT_DIR='./'

while [ $# -gt 0 ]; do
  case $1 in
    -h|--help)
        help
        exit
      ;;

    -o|outdir)
        OUT_DIR=$2;
        shift 2
      ;;

    -d|--dist)
        DIST=$2
        shift 2
      ;;

    -r|--release)
        RELEASE=$2
        shift 2
      ;;

    --ver)
        VER=$2
        shift 2
      ;;

    -b|--binary)
        TYPE='-b'
        shift
      ;;

    --)
        shift
        break
      ;;

    *)
        SRC_DIR=$1
        shift
      ;;

  esac
done


if [ -z "${SRC_DIR}" ]; then
  echo "missing path-to-source operand" >&2
  help
  exit 2
fi


if [ -z "${DIST}" ]; then
  echo "missing dist option"
  help
  exit 2
fi
  

if [ -z "${RELEASE}" ]; then
  echo "missing release option"
  help
  exit 2
fi

if [ ! -d ${OUT_DIR} ]; then
    echo "${OUT_DIR}: No such directory"
    exit 2
fi

OUT_DIR=`cd ${OUT_DIR}; pwd`



if [ -z "$VER" ]; then
   VER=`awk -F'[)( ]' '/set.*RAZOR_VERSION/ {print($3)}' ${SRC_DIR}/CMakeLists.txt`
    #VER=`cd ${SRC_DIR} && git log '--pretty=%ai' -n 1  | awk '{print $1}' | sed -e's/-/./g'`
fi


echo "*******************************"
echo " Name: ${NAME}"
echo " Ver:  ${VER}"
echo " Type: ${TYPE}"
echo " Release: ${DIST} ${RELEASE}"
echo " Src dir: ${SRC_DIR}"
echo " Out dir: ${OUT_DIR}"
echo "*******************************"

mkdir -p ${OUT_DIR}

DIR=${OUT_DIR}/${NAME}-${VER}
rm -rf ${DIR}

cp -r ${SRC_DIR} ${DIR}
rm -rf ${DIR}/.git \
       ${DIR}/build

cd ${DIR}/.. && tar czf ${NAME}_${VER}.orig.tar.gz ${NAME}-${VER}
cp --force -R ${DIR}/distr/deb/$DIST/debian ${DIR}/

DATE=`date -R`
for f in `find ${DIR}/debian -type f `; do
    sed -i \
        -e"s/%NAME%/${NAME}/g"  \
        -e"s/%VERSION%/${VER}/g" \
        -e"s/%DIST%/${RELEASE}/g" \
        -e"s/%DATE%/${DATE}/g" \
        $f
done

cd ${DIR} && debuild ${TYPE} -rfakeroot


