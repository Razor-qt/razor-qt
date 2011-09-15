#!/bin/sh


NAME='razor-qt'
SRC_DIR=''
DIST=''
VER=''
TYPE='-S'

while [ $# -gt 0 ]; do
  case $1 in
    -d|--dist)
        DIST=$2
        shift 2
      ;;

    -r|--releases)
        RELEASES=$2
        shift 2
      ;;

    --ver)
        VER=$2
        shift 2
      ;;

    --key)
        KEY=$2
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
#        echo "Unknown option $1" 1>&2
#        error=1
        break
      ;;

  esac
done



if [ -z "${DIST}" ]; then
  echo "!11"
  #echo "Usage: makeDeb.sh --dist DISTRIBUTIVE --releases 'RELEASE1 RELEASE2 ...' [--ver VERSION] [--binary]" >&2
  error=2
fi
  

if [ -z "${RELEASES}" ]; then
  echo "!22"
  #echo "Usage: makeDeb.sh --dist DISTRIBUTIVE --releases 'RELEASE1 RELEASE2 ...' [--ver VERSION] [--binary]" >&2
  error=2
fi

if [ -z "${SRC_DIR}" ]; then
  echo "!33" >&2
  error=2
fi

if [ -z "$VER" ]; then
    #VER=`LANG=C svn info ${SVN} | grep '^Revision:' | awk '{print $2}'`
    VER=`cd ${SRC_DIR} && git log '--pretty=%ai' -n 1  | awk '{print $1}' | sed -e's/-/./g'`
    NAME=$NAME-git
fi

if [ -z "$VER" ]; then
  echo "" >&2
  error=3 
fi

if [ "$error" ]; then
    echo "Usage: makeDeb.sh --dist DISTRIBUTIVE --releases 'RELEASE1 RELEASE2 ...' [--ver VERSION] [--binary]  <path-to-source>" >&2
    exit $error
fi

#ROOT_DIR=`pwd`
#while [ ! -d "${ROOT_DIR}/.git" ]; do
#  ROOT_DIR=`dirname $ROOT_DIR`
#  if [ "$ROOT_DIR" = "/" ]; then
#    echo "Not a git repository (or any of the parent directories)." >&2
#    exit 2
#  fi
#done

echo "*******************************"
echo " Name: ${NAME}"
echo " Ver:  ${VER}"
echo " Type: ${TYPE}" 
echo " Releases: ${RELEASES}"
echo " Src dir: ${SRC_DIR}"
echo "*******************************"

WORK_DIR=`pwd`
DIR=${NAME}-${VER}

#mkdir -p ${DIR}

cd ${SRC_DIR} && git checkout-index --all --force --prefix=${WORK_DIR}/${DIR}/ || { echo "Can't export sources" >&2; exit 2; }
cd ${WORK_DIR} && tar czf ${NAME}_${VER}.orig.tar.gz ${NAME}-${VER}

DATE=`date -R`
for RELEASE in ${RELEASES}; do
    echo "*******************************"
    echo " Build $DIST/$RELEASE"
    echo "*******************************"

    SRC_DEB_DIR=${DIR}/distr/$DIST/debian
    [ -d ${DIR}/debian ] && rm -r ${DIR}/debian
    cp --force -R ${SRC_DEB_DIR} ${DIR}/ || exit 1

    for f in `find ${DIR}/debian -type f `; do
        sed -i \
            -e"s/%NAME%/${NAME}/g"  \
            -e"s/%VERSION%/${VER}/g" \
            -e"s/%DIST%/${RELEASE}/g" \
            -e"s/%DATE%/${DATE}/g" \
            $f
    done

    DEBUILD_OPT="-rfakeroot -sa"
    if [ -n "${KEY}" ]; then
      DEBUILD_OPT="${DEBUILD_OPT} -k${KEY}"
    fi

    cd ${DIR} && debuild ${TYPE} ${DEBUILD_OPT}
    cd $WORK_DIR
done



