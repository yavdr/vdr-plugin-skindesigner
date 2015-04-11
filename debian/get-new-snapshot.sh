echo Downloading latest from git repository...
#set -x

BUILDDISTRIBUTION=$1
GITSOURCE='git://projects.vdr-developer.org/vdr-plugin-skindesigner.git'
PPAENV='yavdr'

DEB_SOURCE_PACKAGE=$(egrep '^Source: ' debian/control | cut -f 2 -d ' ')
DISTRIBUTION=$(dpkg-parsechangelog | grep ^Distribution: | sed -e 's/^Distribution:\s*//')
# VERSION_UPSTREAM=`dpkg-parsechangelog | grep ^Version: | sed -e 's/^Version:\s*//' -e 's/-[^-]*$//' -e 's/\.git.*//'`
VERSION_UPSTREAM=$(dpkg-parsechangelog | grep ^Version: | sed -e 's/^Version:\s*//' -e 's/\.git.*//' -e 's/~*//g')
SUBVERSION=$(dpkg-parsechangelog | grep ^Version: | sed -e 's/^Version:\s*//' -e 's/~.*//g' | awk -F'-' '{ print $4 }' | sed -e "s/${PPAENV}.*//g")
GIT_SHA_OLD=$(git show --pretty=format:"%h" --quiet | head -1 || true)

if [ -d ${DEB_SOURCE_PACKAGE} ] ; then
        rm -rf ${DEB_SOURCE_PACKAGE}
fi

if [ -d ".git" ] ; then
	git pull
	GITCHANGE=$(git log --pretty=format:"%h: %s" -1)
fi

VERSION_SOURCE=$(grep 'static const char \*VERSION' skindesigner.c | sed -e 's/^.*=//g'  -e 's/[";]//g' -e 's/\s//g')
VERSION_DATE=$(/bin/date --utc +%0Y%0m%0d)
GITHEAD=$(git rev-list HEAD | wc -l)
GITBUILD=$(printf '%04d' "$GITHEAD")
GITVER=$(git describe | cut -f 1 -d '-') 
GITSUBVER=$(git describe | cut -f 2 -d '-')
GITDESCBUILD=$(git describe | cut -f 3 -d '-')
BUILD=$(/bin/date --utc +%H%M)

# VERSION_FULL="${GITVER}.git${VERSION_DATE}.${BUILD}"
VERSION_FULL="$(git describe)"

git clone --depth 1 ${GITSOURCE} ${DEB_SOURCE_PACKAGE}

cd ${DEB_SOURCE_PACKAGE}
GIT_SHA=$(git show --pretty=format:"%h" --quiet | head -1 || true)
cd ..

if [ ! $(echo $VERSION_FULL | grep -) ] ; then
        VERSION_FULL="${VERSION_FULL}-0-g${GIT_SHA}"
fi
# VERSION_FULL="${VERSION_FULL}.${VERSION_DATE}.${BUILD}"

if [ "x${GIT_SHA_OLD}" == "x${GIT_SHA}" ] ; then
        echo "Keine neue Version von ${DEB_SOURCE_PACKAGE} gefunden: ${GIT_SHA_OLD} = ${GIT_SHA}" | tee ${CHKFILE}
fi

cp ./debian/changelog ./debian/changelog.${DISTRIBUTION}

if [ $DISTRIBUTION != 'trusty' ] ; then
        DISTRIBUTION='trusty'
else
        DISTRIBUTION='precise'
fi

if [ ! -z $BUILDDISTRIBUTION ] ; then
	DISTRIBUTION=$BUILDDISTRIBUTION
fi

cp ./debian/changelog.${DISTRIBUTION} ./debian/changelog

test=$(grep ${GIT_SHA} debian/changelog | grep ${DISTRIBUTION})
if [ $? -eq 0 ] ; then
	SUBVERSION=$((SUBVERSION +1))
else
	SUBVERSION=0
fi

# libskindesignerapi
LIBBUILD=$(cat ./debian/libbuild.${DISTRIBUTION})
LIBVERSION="$(grep ^MAJOR ./libskindesignerapi/Makefile | awk '{ print $3 }').$(grep ^MINOR ./libskindesignerapi/Makefile | awk '{ print $3 }')"

test=$(grep "${LIBVERSION}-0${PPAENV}" ./debian/rules)
if [ $? -eq 0 ] ; then
	test=$(echo $test | awk -F'-- -v' '{ print $2 }' | awk '{ print $1 }')
	OLDLIBBUILD=$(echo $test | sed -e "s/${LIBVERSION}-0${PPAENV}//g" | awk -F'~' '{ print $1 }')
	OLDLIBDISTRIBUTION=$(echo $test | sed -e "s/${LIBVERSION}-0${PPAENV}//g" | awk -F'~' '{ print $2 }')
	if [ "${OLDLIBDISTRIBUTION}" == "${DISTRIBUTION}" ] ; then
		if [ "${OLDLIBBUILD}" -eq "${LIBBUILD}" ] ; then
        		LIBBUILD=$((LIBBUILD +1))
		fi
	fi
else
	echo "rules file are not ok"
	exit 1
fi

RULESVERSION="${LIBVERSION}-0${PPAENV}${LIBBUILD}~${DISTRIBUTION}"
echo ${LIBBUILD} > ./debian/libbuild.${DISTRIBUTION}
# modifiy rules
cat ./debian/rules.templ | sed -e "s/<version>/${RULESVERSION}/g" > ./debian/rules


echo "${DEB_SOURCE_PACKAGE}: ${VERSION_FULL}-${SUBVERSION}${PPAENV}0~${DISTRIBUTION}"
echo "libskindesignerapi:      ${RULESVERSION}"

# exit 0

DEBSRCPKGFILE="../${DEB_SOURCE_PACKAGE}_${VERSION_FULL}.orig.tar.xz"

if [ ! -f ${DEBSRCPKGFILE} ] ; then
	echo $DEBSRCPKGFILE
	tar --exclude=.git --exclude=debian -cJf ${DEBSRCPKGFILE} ${DEB_SOURCE_PACKAGE}
fi
rm -rf ${DEB_SOURCE_PACKAGE}

dch -b -D ${DISTRIBUTION} -v "${VERSION_FULL}-${SUBVERSION}${PPAENV}0~${DISTRIBUTION}" "${GITCHANGE}"
dch -a "${GIT_SHA}: libskindesignerapi: v${LIBVERSION}"

exit 0
