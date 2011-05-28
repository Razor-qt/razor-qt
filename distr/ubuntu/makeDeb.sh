#!/bin/sh


NAME='razor-qt'
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

    -b|--binary)
        TYPE='-b'
        shift
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
    echo "Usage: makeDeb.sh [--ver VERSION] [--binary]"
    exit
fi

if [ -z "$VER" ]; then
    VER=`LANG=C svn info ${SVN} | grep '^Revision:' | awk '{print $2}'`
    NAME=$NAME-svn
fi

echo "*******************************"
echo " Name: ${NAME}"
echo " Ver:  ${VER}"
echo " Type: ${TYPE}" 
echo " Releases: ${RELEASES}"
echo "*******************************"

DIR=${NAME}-${VER}
svn export --force ${SVN} ${DIR}
tar czf ${NAME}_${VER}.orig.tar.gz ${DIR}

DATE=`date -R`
CUR_DIR=`pwd`
for RELEASE in $RELEASES; do
    echo "*******************************"
    echo " Build $RELEASE"
    echo "*******************************"

    SRC_DEB_DIR=${DIR}/distr/ubuntu/debian
    cp --force -R ${SRC_DEB_DIR} ${DIR}/ || exit 1

    for f in `find ${DIR}/debian -type f `; do
        sed -i \
            -e"s/%NAME%/${NAME}/g"  \
            -e"s/%VERSION%/${VER}/g" \
            -e"s/%DIST%/${RELEASE}/g" \
            -e"s/%DATE%/${DATE}/g" \
            $f
    done

    cd ${DIR} && debuild ${TYPE} -rfakeroot -sa -k${KEY}
    cd $CUR_DIR
done

# Create send2launchpad.sh script 
echo "dput ppa:razor-qt/ppa ${NAME}_${VER}*_source.changes" > send2launchpad.sh
chmod u+x send2launchpad.sh



