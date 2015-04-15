#!/bin/bash

QWANA_BASE_DIR=`dirname ${0}`/..

QWANA_VERSION_H=${1:-./QwSVNVersion.h}
mkdir -p `dirname ${QWANA_VERSION_H}`

QWANA_SVN_REVISION=`svnversion ${QWANA_BASE_DIR} 2>/dev/null || echo "unknown_revision"`
QWANA_SVN_REVISION_OLD=`{ test -e ${QWANA_VERSION_H} && grep "QWANA_SVN_REVISION" ${QWANA_VERSION_H} ; } | awk -F'"' '{print $2}'`
QWANA_SVN_LASTREVISION=`{ svn info ${QWANA_BASE_DIR} 2>/dev/null || echo "Last Changed Rev: unknown_revision" ; } | awk -F":" '$1=="Last Changed Rev"{print $2}'`
QWANA_SVN_URL=`{ svn info ${QWANA_BASE_DIR} 2>/dev/null || echo "URL: unknown_URL" ; } | awk '$1=="URL:"{print $2}'`

# Remove spaces
QWANA_SVN_URL="${QWANA_SVN_URL//[[:space:]]/}"
QWANA_SVN_REVISION="${QWANA_SVN_REVISION//[[:space:]]/}"
QWANA_SVN_REVISION_OLD="${QWANA_SVN_REVISION_OLD//[[:space:]]/}"
QWANA_SVN_LASTREVISION="${QWANA_SVN_LASTREVISION//[[:space:]]/}"

if [ "${QWANA_SVN_REVISION}" != "${QWANA_SVN_REVISION_OLD}" ] ; then
  echo "Generating ${QWANA_VERSION_H} with revision string ${QWANA_SVN_REVISION}."
  echo "#define QWANA_SVN_URL \"${QWANA_SVN_URL}\"" > "${QWANA_VERSION_H}"
  echo "#define QWANA_SVN_REVISION \"${QWANA_SVN_REVISION}\"" >> "${QWANA_VERSION_H}"
  echo "#define QWANA_SVN_LASTCHANGEDREVISION \"${QWANA_SVN_LASTREVISION}\"" >> "${QWANA_VERSION_H}"
else
  echo "${QWANA_VERSION_H} is up to date."
fi
