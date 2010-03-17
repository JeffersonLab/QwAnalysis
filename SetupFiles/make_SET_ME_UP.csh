#!/bin/csh
#

set orig_dir = `pwd`

cd `dirname $0`
set local_path = `pwd`

##  Backup the old versions of SET_ME_UP
foreach setupfile (`ls -rt SET_ME_UP*`)
    mv $setupfile ${setupfile}_
end

setenv QWANALYSIS `echo $local_path | sed 's/\/SetupFiles//'`

cat >! SET_ME_UP.csh << END
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

cat >! SET_ME_UP.bash << END
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

