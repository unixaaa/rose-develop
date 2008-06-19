#!/bin/bash

# Stub file to include settings for roseFreshTest-common

source /usr/apps/graphviz/2.6/setup.sh
# source /usr/apps/python/2.5/setup.sh
export PATH="/usr/apps/java/jdk1.5.0_11/bin:$PATH"
export LD_LIBRARY_PATH="/usr/apps/java/jdk1.5.0_11/lib:/usr/apps/java/jdk1.5.0_11/jre/lib/amd64/server:$LD_LIBRARY_PATH"
export JAVA_HOME=/usr/apps/java/jdk1.5.0_11
export CLASSPATH="/home/willcock2/antlr-3.0.1/lib/antlr-2.7.7.jar:/home/willcock2/antlr-3.0.1/lib/antlr-3.0.1.jar:/home/willcock2/antlr-3.0.1/lib/antlr-runtime-3.0.1.jar:/home/willcock2/antlr-3.0.1/lib/stringtemplate-3.1b1.jar:/home/dquinlan/ROSE/OpenFortranParser/ofp_install/lib/OpenFortranParser.jar:."
export LD_LIBRARY_PATH="/home/dquinlan/ROSE/OpenFortranParser/ofp_install/lib/:$LD_LIBRARY_PATH"
# source /usr/apps/gcc/4.2.2/setup.sh
export PATH="/usr/apps/gcc/4.2.2-64bit/bin:$PATH"
export LD_LIBRARY_PATH="/usr/apps/gcc/4.2.2-64bit/lib:$LD_LIBRARY_PATH"
export PATH="/usr/apps/automake/1.9.6/bin:$PATH"
export PATH="/home/willcock2/swig-install/bin:$PATH"
export BOOST_ROOTX="/home/willcock2/boost-1.35-install64/"
export LD_LIBRARY_PATH="${BOOST_ROOTX}/lib:${LD_LIBRARY_PATH}"
# export LIBRARY_PATH="/usr/lib64"
# export PATH="/home/willcock2/tcl8.5-install/bin:$PATH"
# export LD_LIBRARY_PATH="/home/willcock2/tcl8.5-install/lib:$LD_LIBRARY_PATH"
# export PATH="/home/willcock2/binutils-install/bin/:$PATH"
# export LD_LIBRARY_PATH="/home/willcock2/binutils-install/lib/:$LD_LIBRARY_PATH"

ROSE_TOP="/export/0/tmp.willcock2/svn-test-rose/$$/"

CONFIGURE_FLAGS=(--with-edg_source_code=true --enable-dq-developer-tests --with-ROSE_LONG_MAKE_CHECK_RULE=yes --with-CXX_DEBUG=-g --with-C_DEBUG=-g --with-CXX_WARNINGS=-Wall --with-CXXFLAGS=-fPIC --with-CFLAGS=-fPIC --prefix=${ROSE_TOP}/install --with-boost=${BOOST_ROOTX} "$@")

ROSE_SVNROOT=file:///usr/casc/overture/ROSE/svn/ROSE/trunk/ROSE
MAILADDRS="willcock2@llnl.gov liaoch@llnl.gov dquinlan@llnl.gov andreas.saebjoernsen@gmail.com yuan5@llnl.gov"

# the right version of subversion is essential!!
SVN=/home/willcock2/svn-install/bin/svn

MAKEFLAGS="-j8"

set -e
set -o pipefail
