#!/bin/csh
#

if (-x $0) then
    ##  Check to see if $0 is the name of an executable; if it
    ##  is, we'll use it's directory as our base seach path.
    set search_dir = `dirname $0`
else
    ##  We've probably sourced this file, or are doing some other
    ##  crazy thing.
    ##  Let's try to do the best we can to complete the operation
    ##  we "ought" to do.
    set search_dir = `pwd`
endif
#  Find the full path which contains the make_SET_ME_UP.csh file
set fullname   = `find $search_dir -name make_SET_ME_UP.csh`
cd `dirname $fullname`
set local_path = `pwd`
cd -

#  Execute the bash make_SET_ME_UP
${local_path}/make_SET_ME_UP.bash

