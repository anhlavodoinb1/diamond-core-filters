#
#  SnapFind
#  An interactive image search application
#  Version 1
#
#  Copyright (c) 2002-2005 Intel Corporation
#  All Rights Reserved.
#
#  This software is distributed under the terms of the Eclipse Public
#  License, Version 1.0 which can be found in the file named LICENSE.
#  ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
#  RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
#

AC_DEFUN([ADD_LIB_SEARCH],
    [
       LIB_SEARCH_DIRS="${LIB_SEARCH_DIRS} $1"
    ])

#
# this is a work around for gcc 4.0.  There is a big design
# flaw that does not let you link against static libraries when
# building .so's to ship as the searchlets.  To get around this
# we find all the libraries and include them as part of the link
# instead of using the -l option.  Yuck, but until they fix the loader...
#

AC_DEFUN([CHECK_STATIC_LIB],
   [
    found_static_lib="no";
    for dir in ${LIB_SEARCH_DIRS}; do
        if test -f "$dir/$1"; then
    	    STATIC_LIBS="$STATIC_LIBS $dir/$1";
	    found_static_lib="yes";
            break;
        fi
    done
    if test x_$found_static_lib != x_yes; then
	AC_MSG_ERROR(Missing $1: try using --with-staticlib option)
    fi
    AC_SUBST(STATIC_LIBS)
    ])
