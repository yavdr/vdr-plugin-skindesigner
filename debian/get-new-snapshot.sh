echo Downloading latest from git repository...
# set -x

BUILDDISTRIBUTION=$1
GITSOURCE='git://projects.vdr-developer.org/vdr-plugin-skindesigner.git'

DEB_SOURCE_PACKAGE=$(egrep '^Source: ' debian/control | cut -f 2 -d ' ')
DISTRIBUTION=$(dpkg-parsechangelog | grep ^Distribution: | sed -e 's/^Distribution:\s*//')
VERSION_UPSTREAM=$(dpkg-parsechangelog | grep ^Version: | sed -e 's/^Version:\s*//' -e 's/\.git.*//' -e 's/~*//g')
SUBVERSION=$(dpkg-parsechangelog | grep ^Version: | sed -e 's/^Version:\s*//' -e 's/~.*//g' | awk -F'-' '{ print $4 }' | sed -e 's/yavdr.*//g')
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

test=$(grep ${GIT_SHA} debian/changelog | grep $DISTRIBUTION)
if [ $? -eq 0 ] ; then
	SUBVERSION=$((SUBVERSION +1))
else
	SUBVERSION=0
fi

echo "${VERSION_FULL}-${SUBVERSION}yavdr0~${DISTRIBUTION}"

# exit 0

DEBSRCPKGFILE="../${DEB_SOURCE_PACKAGE}_${VERSION_FULL}.orig.tar.xz"

if [ ! -f ${DEBSRCPKGFILE} ] ; then
	tar --exclude=.git --exclude=debian -cJf ${DEBSRCPKGFILE} ${DEB_SOURCE_PACKAGE}
fi
rm -rf ${DEB_SOURCE_PACKAGE}

dch -b -D ${DISTRIBUTION} -v "${VERSION_FULL}-${SUBVERSION}yavdr0~${DISTRIBUTION}" "${GITCHANGE}"

exit 0
