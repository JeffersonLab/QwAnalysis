#!/bin/bash
#

if [[ -x $0 ]]; then
    ##  Check to see if $0 is the name of an executable; if it
    ##  is, we'll use it's directory as our base seach path.
    search_dir="`dirname $0`"
else
    ##  We've probably sourced this file, or are doing some other
    ##  crazy thing.
    ##  Let's try to do the best we can to complete the operation
    ##  we "ought" to do.
    search_dir="`pwd`"
fi
#  Find the full path which contains the make_SET_ME_UP.bash file
fullname="`find $search_dir -name make_SET_ME_UP.bash`"
cd `dirname $fullname`
local_path="`pwd`"
cd -

##  Backup the old versions of SET_ME_UP
for setupfile in `find $local_path -name SET_ME_UP\* -print | sort -r` ; do
	 mv ${setupfile} ${setupfile}_
done

export QWANALYSIS=`echo $local_path | sed 's/\/SetupFiles//'`

cat > $local_path/SET_ME_UP.csh << END
#!/bin/csh
#
set analyzer_version = $QWANALYSIS

###
###  DO NOT CHANGE THE COMMANDS BELOW!!!
###
setenv QWANALYSIS \$analyzer_version

source \${QWANALYSIS}/SetupFiles/.QwSetup.csh

unset analyzer_version

END

cat > $local_path/SET_ME_UP.bash << END
#!/bin/bash
#
analyzerversion=$QWANALYSIS

###
###  DO NOT CHANGE THE COMMANDS BELOW!!!
###

export QWANALYSIS=\$analyzerversion

. \$QWANALYSIS/SetupFiles/.QwSetup.bash

unset analyzerversion

END

cd $orig_dir
