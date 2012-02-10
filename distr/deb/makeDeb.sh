#!/bin/bash

function help {
  cat << HELP_TEXT
Usage: makeDeb.sh [otions] <path-to-source>

Options
  -h|--help             display this message
  -o|--outdirt=DIR      write result to DIR, home directory by default
  -r|--release=RELEASE  release name (sid, maveric, natty etc.), autodetect if ommited
  -ver=VERSION          razor version
  -S|--sign             sign a result files
  -s|--source           build a source package, if ommited build a binary package
HELP_TEXT
}

function checkIf
{
  shift
  for i in $@ ; do
    [ "$i" = "${RELEASE}" ] && return 0
  done

  return 1
}


function prepareFile
{
  local file=$1
  local skip=0
  while  IFS='' read "line"; do
    local cmd=`echo $line | awk '{print $1 }' | tr '[:lower:]' '[:upper:]'` 
    case $cmd in	
      %IF)
        checkIf $line || skip=1
        ;;
		
      %IFNOT)
        checkIf $line && skip=1
        ;;

      %ELSE)
        let "skip = 1-$skip"
        ;;
			
      %ENDIF)
        skip=0
        ;;
			
      *)
        if [ "$skip" = 0 ]; then
			echo "$line" | sed            \
				-e"s/%NAME%/${NAME}/g"    \
				-e"s/%VERSION%/${VER}/g"  \
				-e"s/%DIST%/${RELEASE}/g" \
				-e"s/%DATE%/${DATE}/g"
		fi
        ;;
    esac
		
  done < "${file}"
}


NAME='razorqt'
TYPE='-b'
SIGN='-uc -us'
DEST_DIR=${HOME}
SRC_DIR="../.."

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

    -s|--source)
        TYPE='-S'
        shift
      ;;

    -S|--sign)
        SIGN=''
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

SRC_DIR=`cd ${SRC_DIR}; pwd`

if [ -z "${RELEASE}" ]; then
    RELEASE=`awk -F"=" '/DISTRIB_CODENAME=/ {print($2)}' /etc/lsb-release`
fi

if [ -z "${RELEASE}" ]; then
    echo "missing release option"
    help
    exit 2
fi

if [ ! -d ${DEST_DIR} ]; then
    echo "${DEST_DIR}: No such directory"
    exit 2
fi

if [ -z "$VER" ]; then
    MAJOR_VER=`awk -F'[)( ]' '/set\s*\(MAJOR_VERSION / {print($3)}' ${SRC_DIR}/CMakeLists.txt`
    MINOR_VER=`awk -F'[)( ]' '/set\s*\(MINOR_VERSION / {print($3)}' ${SRC_DIR}/CMakeLists.txt`
    PATCH_VER=`awk -F'[)( ]' '/set\s*\(PATCH_VERSION / {print($3)}' ${SRC_DIR}/CMakeLists.txt`
    VER="${MAJOR_VER}.${MINOR_VER}.${PATCH_VER}"
fi

OUT_DIR=`cd ${DEST_DIR}; pwd`/razorqt_${VER}_deb



echo "*******************************"
echo " Name: ${NAME}"
echo " Ver:  ${VER}"
[ "${TYPE}" = "-b" ] && echo " Type: binary"
[ "${TYPE}" = "-S" ] && echo " Type: source"
echo " Release: ${RELEASE}"
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

# Debin directory .....................
mkdir -p ${DIR}/debian
mkdir -p ${DIR}/debian/source
DATE=`date -R`
for src in `find ${DIR}/distr/deb/debian -type f `; do
    dest=`echo $src | sed -e's|/distr/deb||'`
    prepareFile "${src}" > ${dest}
done
# Debin directory .....................


cd ${DIR} && debuild ${TYPE} ${SIGN} -rfakeroot

echo "................................."
echo "Check files:"
PKGS=`awk '/Package:/ {print $2}' ${DIR}/debian/control`

for file in `find ${DIR}/debian/tmp -type f 2>/dev/null`; do
    file=`echo $file | sed -e"s|${DIR}/debian/tmp||"`
    #echo $file
    pkgNames=''
    let 'pkgCount=0'

    for pkg in ${PKGS}; do
        if [ `ls "${DIR}/debian/${pkg}$file" 2>/dev/null` ]; then
            let 'pkgCount++'
            pkgNames="${pkgNames}\n\t${pkg}"
        fi
    done

    if [ $pkgCount -eq 0 ]; then
        echo -e "Missing file: ${file}";

    elif [ $pkgCount -gt 1 ]; then
        echo -e "Douplicates:  ${file}$pkgNames"
    fi

done


