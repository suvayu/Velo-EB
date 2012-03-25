#!/bin/bash
#
# This script updates the GitHub page for this project
#
#

TOPDIR=$(dirname $0)

[[ ! -d ${TOPDIR}/.git ]] && \
    echo Could not find GIT_DIR && exit 1

# cleanup
pushd ${TOPDIR}
git stash

# update GitHub pages
git checkout gh-pages
git rebase master

# update doxygen HTML documentation and commit changes
doxygen docs/Velo-EB-doxy.conf > /dev/null
git add docs/html/ > /dev/null
git commit -m "Update doxygen html documentation" > /dev/null

# # update doxygen PDF documentation and commit changes
# pushd ${TOPDIR/docs/latex} && (make; make; make;) > /dev/null
# git add refman.pdf
# git commit -m "Update doxygen pdf documentation"

# push changes
git push -f origin gh-pages
git checkout master
git stash pop -q > /dev/null

popd
