#!/bin/sh


NAME='razor'
KEY='29EE0D60'
SVN='https://razor-qt.svn.sourceforge.net/svnroot/razor-qt/trunk'
RELEASES="maverick natty"

VER=''
SVN_DIR=''
TYPE='-S'

while [ $# -gt 0 ]; do
  case $1 in

    --ver)
        VER=$2
        shift 2
      ;;

    --type)
        TYPE=$2
        shift 2
      ;;

    --)
        shift
        break
      ;;

    *)
        echo "Unknown option $1" 1>&2
        error=1
        break
      ;;

  esac
done



if [ "$error" ]; then
    echo "Usage: makeDeb.sh [--ver VERSION] [--type TYPE]"
    exit
fi

if [ -z "$VER" ]; then
    VER=`LANG=C svn info ${SVN} | grep '^Revision:' | awk '{print $2}'`
    NAME=$NAME-svn
fi

echo "*******************************"
echo " Dist: ${DIST}"
echo " Name: ${NAME}"
echo " Ver:  ${VER}"
echo " RELEASES: ${RELEASES}"
echo "*******************************"

DIR=${NAME}-${VER}
svn export --force ${SVN} ${DIR}
tar czf ${NAME}_${VER}.orig.tar.gz ${DIR}

SRC_DEB_DIR=${SVN_DIR}/distr/ubuntu/debian

#cp --force -R ${SRC_DEB_DIR} ${DIR}/debian
#DATE=`date -R`
#for f in `find ${SRC_DEB_DIR} -type f \;`; do
#   sed -i \
#       -e"s/%NAME%/${NAME}/g"  \
#       -e"s/%VERSION%/${VER}/g" \
#       -e"s/%DIST%/${DIST}/g" \
#       -e"s/%DATE%/${DATE}/g" \
#       ${SRC_DEB_DIR}/$f
#done

#cd ${DIR} && dpkg-buildpackage -rfakeroot 
#cd ${DIR} && debuild ${TYPE} -sa -k${KEY}



