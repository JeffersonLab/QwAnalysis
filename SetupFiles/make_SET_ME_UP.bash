#!/bin/bash
#

orig_dir=`pwd`

cd `dirname $0`
local_path=`pwd`

export QWANALYSIS=`echo $local_path | sed 's/\/SetupFiles//'`

cat > SET_ME_UP.csh << END
#!/bin/csh
#
set analyzer_version = $QWANALYSIS

###
###  DO NOT CHANGE THE COMMANDS BELOW!!!
###
	setenv QWANALYSIS \$analyzer_version
	unsetenv RCLD_LIBRARY_PATH
	unsetenv RCQWANALYSIS
	unsetenv RCQWANALYSIS_INIT

	source \${QWANALYSIS}/SetupFiles/.QwSetup.csh

unset analyzer_version

END

cat > SET_ME_UP.bash << END
#!/bin/bash
#
analyzerversion=$QWANALYSIS

###
###  DO NOT CHANGE THE COMMANDS BELOW!!!
###

export QWANALYSIS=\$analyzerversion
unset RCLD_LIBRARY_PATH
unset RCQWANALYSIS
unset RCQWANALYSIS_INIT

. \$QWANALYSIS/SetupFiles/.QwSetup.bash

unset analyzerversion

END

cd $orig_dir
