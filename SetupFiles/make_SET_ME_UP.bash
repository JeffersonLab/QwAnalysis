#!/bin/bash
#

orig_dir=`pwd`
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
cd $OLDPWD

##  Backup the old versions of SET_ME_UP
for setupfile in `find $local_path -name SET_ME_UP\* -print | sort -r` ; do
	 mv ${setupfile} ${setupfile}_
done

export QWANALYSIS=`echo $local_path | sed 's/\/SetupFiles//'`



###  Try to find MYSQLPP_INC_DIR
###  Try the following order if not set:  
###      /usr/include/mysql++,
###      /usr/local/include/mysql++,
###      /group/qweak/spayde/local/include/mysql++
if [[ -n ${MYSQLPP_INC_DIR} ]]; then
  if [[ ! -d ${MYSQLPP_INC_DIR} ]]; then
    echo "Directory ${MYSQLPP_INC_DIR} does not exist; unset variable MYSQLPP_INC_DIR"
    unset MYSQLPP_INC_DIR
  fi
fi
if [[ -z ${MYSQLPP_INC_DIR} ]]; then
  nextserchpath=/usr/include/mysql++
  echo "MYSQLPP_INC_DIR not defined; trying ${nextserchpath}."
  if [[ -d ${nextserchpath} ]]; then
    export MYSQLPP_INC_DIR=${nextserchpath}
    echo "Setting MYSQLPP_INC_DIR to ${MYSQLPP_INC_DIR}."
    export MYSQLPP_LIB_DIR=/usr/lib
  fi
fi
if [[ -z ${MYSQLPP_INC_DIR} ]]; then
  nextserchpath=/usr/local/include/mysql++
  echo "MYSQLPP_INC_DIR not defined; trying ${nextserchpath}."
  if [[ -d ${nextserchpath} ]]; then
    export MYSQLPP_INC_DIR=${nextserchpath}
    echo "Setting MYSQLPP_INC_DIR to ${MYSQLPP_INC_DIR}."
    export MYSQLPP_LIB_DIR=/usr/local/lib
  fi
fi
if [[ -z ${MYSQLPP_INC_DIR} ]]; then
  nextserchpath=/group/qweak/spayde/local/include/mysql++
  echo "MYSQLPP_INC_DIR not defined; trying ${nextserchpath}."
  if [[ -d ${nextserchpath} ]]; then
    export MYSQLPP_INC_DIR=${nextserchpath}
    echo "Setting MYSQLPP_INC_DIR to ${MYSQLPP_INC_DIR}."
    export MYSQLPP_LIB_DIR=/group/qweak/spayde/local/lib
  fi
fi

#  Verify the exisitence of the MYSQLPP_LIB_DIR directory
if [[ -n ${MYSQLPP_LIB_DIR} ]]; then
  if [[ ! -d ${MYSQLPP_LIB_DIR} ]]; then
    echo "Directory ${MYSQLPP_LIB_DIR} does not exist; unset variable MYSQLPP_LIB_DIR"
    unset MYSQLPP_LIB_DIR
  fi
fi

#  Check and report failure to find either MySQL++ directory.
if [[ -z ${MYSQLPP_LIB_DIR} || -z ${MYSQLPP_INC_DIR} ]]; then
  echo "ERROR:  The MySQL++ environment variables could not be set."
  echo "        Please ensure MySQL++ is installed on the system,"
  echo "        set the MYSQLPP_LIB_DIR and MYSQLPP_INC_DIR "
  echo "        environment variables, and re-run this program."
  exit 1
fi


###  Create the setup scripts.

cat > $local_path/SET_ME_UP.csh << END
#!/bin/csh
#
set analyzer_version = $QWANALYSIS

###
###  DO NOT CHANGE THE COMMANDS BELOW!!!
###

setenv QWANALYSIS \$analyzer_version

setenv MYSQLPP_INC_DIR $MYSQLPP_INC_DIR
setenv MYSQLPP_LIB_DIR $MYSQLPP_LIB_DIR

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

export MYSQLPP_INC_DIR=$MYSQLPP_INC_DIR
export MYSQLPP_LIB_DIR=$MYSQLPP_LIB_DIR

. \$QWANALYSIS/SetupFiles/.QwSetup.bash

unset analyzerversion

END

cd $orig_dir
