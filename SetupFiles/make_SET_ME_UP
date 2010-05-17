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



###  Determine machine type (64-bit)
###
###
if [[ "`uname -m`" == "x86_64" ]]; then
  lib=lib64
else
  lib=lib
fi
echo "Libraries will be searched for in ${lib}."



###  Try to find MYSQL_INC_DIR,
###  in the following order if not set:
###      /usr/include/mysql,
###      /usr/local/include/mysql

#  Verify the existence of the MYSQL_INC_DIR directory
if [[ -n ${MYSQL_INC_DIR} ]]; then
  if [[ ! -d ${MYSQL_INC_DIR} ]]; then
    echo "Directory ${MYSQL_INC_DIR} does not exist."
    echo "Unsetting variable MYSQL_INC_DIR."
    unset MYSQL_INC_DIR
  fi
fi
#  Try /usr/include/mysql,
if [[ -z ${MYSQL_INC_DIR} ]]; then
  nextsearchpath=/usr/include/mysql
  echo "MYSQL_INC_DIR not defined; trying ${nextsearchpath}."
  if [[ -d ${nextsearchpath} ]]; then
    export MYSQL_INC_DIR=${nextsearchpath}
    echo "Setting MYSQL_INC_DIR to ${MYSQL_INC_DIR}."
  fi
fi
#  Try /usr/local/include/mysql,
if [[ -z ${MYSQL_INC_DIR} ]]; then
  nextsearchpath=/usr/local/include/mysql
  echo "MYSQL_INC_DIR not defined; trying ${nextsearchpath}."
  if [[ -d ${nextsearchpath} ]]; then
    export MYSQL_INC_DIR=${nextsearchpath}
    echo "Setting MYSQL_INC_DIR to ${MYSQL_INC_DIR}."
  fi
fi

#  Verify the existence of the MYSQL_LIB_DIR directory
if [[ -n ${MYSQL_LIB_DIR} ]]; then
  if [[ ! -d ${MYSQL_LIB_DIR} ]]; then
    echo "Directory ${MYSQL_LIB_DIR} does not exist."
    echo "Unsetting variable MYSQL_LIB_DIR."
    unset MYSQL_LIB_DIR
  fi
fi
#  Try /usr/lib/mysql,
if [[ -z ${MYSQL_LIB_DIR} ]]; then
  nextsearchpath=/usr/${lib}/mysql
  echo "MYSQL_LIB_DIR not defined; trying ${nextsearchpath}."
  if [[ -d ${nextsearchpath} ]]; then
    export MYSQL_LIB_DIR=${nextsearchpath}
    echo "Setting MYSQL_LIB_DIR to ${MYSQL_LIB_DIR}."
  fi
fi
#  Try /usr/local/lib/mysql,
if [[ -z ${MYSQL_LIB_DIR} ]]; then
  nextsearchpath=/usr/local/${lib}/mysql
  echo "MYSQL_LIB_DIR not defined; trying ${nextsearchpath}."
  if [[ -d ${nextsearchpath} ]]; then
    export MYSQL_LIB_DIR=${nextsearchpath}
    echo "Setting MYSQL_LIB_DIR to ${MYSQL_LIB_DIR}."
  fi
fi

#  Check and report failure to find either MySQL directory.
if [[ -z ${MYSQL_LIB_DIR} || -z ${MYSQL_INC_DIR} ]]; then
  echo "ERROR:  The MySQL environment variables could not be set."
  echo "        Please ensure MySQL is installed on the system,"
  echo "        set the MYSQL_LIB_DIR and MYSQL_INC_DIR "
  echo "        environment variables, and re-run this program."
  exit 1
fi




###  Try to find MYSQLPP_INC_DIR and MYSQLPP_LIB_DIR,
###  in the following order if not set:
###      /usr/include/mysql++,
###      /usr/local/include/mysql++,
###      /group/qweak/spayde/local/include/mysql++
###  and
###      /usr/lib
###      /usr/lib/mysql++,
###      /usr/local/include,
###      /usr/local/include/mysql++,
###      /group/qweak/spayde/local/lib

#  Verify the existence of the MYSQLPP_INC_DIR directory
if [[ -n ${MYSQLPP_INC_DIR} ]]; then
  if [[ ! -d ${MYSQLPP_INC_DIR} ]]; then
    echo "Directory ${MYSQLPP_INC_DIR} does not exist."
    echo "Unsetting variable MYSQLPP_INC_DIR."
    unset MYSQLPP_INC_DIR
  fi
fi
#  Try /usr/include/mysql++,
if [[ -z ${MYSQLPP_INC_DIR} ]]; then
  nextsearchpath=/usr/include/mysql++
  echo "MYSQLPP_INC_DIR not defined; trying ${nextsearchpath}."
  if [[ -d ${nextsearchpath} ]]; then
    export MYSQLPP_INC_DIR=${nextsearchpath}
    echo "Setting MYSQLPP_INC_DIR to ${MYSQLPP_INC_DIR}."
  fi
fi
#  Try /usr/local/include/mysql++
if [[ -z ${MYSQLPP_INC_DIR} ]]; then
  nextsearchpath=/usr/local/include/mysql++
  echo "MYSQLPP_INC_DIR not defined; trying ${nextsearchpath}."
  if [[ -d ${nextsearchpath} ]]; then
    export MYSQLPP_INC_DIR=${nextsearchpath}
    echo "Setting MYSQLPP_INC_DIR to ${MYSQLPP_INC_DIR}."
  fi
fi
#  Try /group/qweak/spayde/local/include/mysql++
if [[ -z ${MYSQLPP_INC_DIR} ]]; then
  nextsearchpath=/group/qweak/spayde/local/include/mysql++
  echo "MYSQLPP_INC_DIR not defined; trying ${nextsearchpath}."
  if [[ -d ${nextsearchpath} ]]; then
    export MYSQLPP_INC_DIR=${nextsearchpath}
    echo "Setting MYSQLPP_INC_DIR to ${MYSQLPP_INC_DIR}."
  fi
fi

#  Verify the existence of the MYSQLPP_LIB_DIR directory
if [[ -n ${MYSQLPP_LIB_DIR} ]]; then
  if [[ ! -d ${MYSQLPP_LIB_DIR} ]]; then
    echo "Directory ${MYSQLPP_LIB_DIR} does not exist."
    echo "Unsetting variable MYSQLPP_LIB_DIR."
    unset MYSQLPP_LIB_DIR
  fi
fi
libname=libmysqlpp.so
#  Try /usr/lib,
if [[ -z ${MYSQLPP_LIB_DIR} ]]; then
  nextsearchpath=/usr/${lib}
  echo "MYSQLPP_LIB_DIR not defined; trying ${nextsearchpath}."
  if [[ -d ${nextsearchpath} && -e ${nextsearchpath}/${libname} ]]; then
    export MYSQLPP_LIB_DIR=${nextsearchpath}
    echo "Setting MYSQLPP_LIB_DIR to ${MYSQLPP_LIB_DIR}."
  fi
fi
#  Try /usr/lib/mysql++,
if [[ -z ${MYSQLPP_LIB_DIR} ]]; then
  nextsearchpath=/usr/${lib}/mysql++
  echo "MYSQLPP_LIB_DIR not defined; trying ${nextsearchpath}."
  if [[ -d ${nextsearchpath} && -e ${nextsearchpath}/${libname} ]]; then
    export MYSQLPP_LIB_DIR=${nextsearchpath}
    echo "Setting MYSQLPP_LIB_DIR to ${MYSQLPP_LIB_DIR}."
  fi
fi
#  Try /usr/local/lib
if [[ -z ${MYSQLPP_LIB_DIR} ]]; then
  nextsearchpath=/usr/local/${lib}
  echo "MYSQLPP_LIB_DIR not defined; trying ${nextsearchpath}."
  if [[ -d ${nextsearchpath} && -e ${nextsearchpath}/${libname} ]]; then
    export MYSQLPP_LIB_DIR=${nextsearchpath}
    echo "Setting MYSQLPP_LIB_DIR to ${MYSQLPP_LIB_DIR}."
  fi
fi
#  Try /usr/local/lib/mysql++
if [[ -z ${MYSQLPP_LIB_DIR} ]]; then
  nextsearchpath=/usr/local/${lib}/mysql++
  echo "MYSQLPP_LIB_DIR not defined; trying ${nextsearchpath}."
  if [[ -d ${nextsearchpath} && -e ${nextsearchpath}/${libname} ]]; then
    export MYSQLPP_LIB_DIR=${nextsearchpath}
    echo "Setting MYSQLPP_LIB_DIR to ${MYSQLPP_LIB_DIR}."
  fi
fi
#  Try /group/qweak/spayde/local/lib
if [[ -z ${MYSQLPP_LIB_DIR} ]]; then
  nextsearchpath=/group/qweak/spayde/local/${lib}
  echo "MYSQLPP_LIB_DIR not defined; trying ${nextsearchpath}."
  if [[ -d ${nextsearchpath} && -e ${nextsearchpath}/${libname} ]]; then
    export MYSQLPP_LIB_DIR=${nextsearchpath}
    echo "Setting MYSQLPP_LIB_DIR to ${MYSQLPP_LIB_DIR}."
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



###  Set up ROOT if it isn't done yet.
###  Expect to use /apps/root/PRO

#  Verify the existence of the ROOTSYS directory
if [[ -n ${ROOTSYS} ]]; then
  echo "ROOTSYS already defined: ${ROOTSYS}"
  if [[ ! -d ${ROOTSYS} ]]; then
    echo "Directory ${ROOTSYS} does not exist."
    echo "Unsetting variable ROOTSYS."
    unset ROOTSYS
  fi
fi
#  Try root-config assuming user has setup PATH correctly
if [[ -z ${ROOTSYS} ]]; then
  echo "ROOTSYS not defined; trying root-config..."
  if [[ `which root-config` ]]; then
    export ROOTSYS="`root-config --prefix`"
    echo "Setting ROOTSYS to ${ROOTSYS} using root-config"
  fi
fi
#  Try /apps/root/5.26-00
if [[ -z ${ROOTSYS} ]]; then
  nextsearchpath=/apps/root/5.26-00/root
  echo "ROOTSYS not defined; trying ${nextsearchpath}."
  if [[ -d ${nextsearchpath} ]]; then
    export ROOTSYS=${nextsearchpath}
    echo "Setting ROOTSYS to ${ROOTSYS}."
  fi
fi
#  Try /apps/root/PRO
if [[ -z ${ROOTSYS} ]]; then
  nextsearchpath=/apps/root/PRO/root
  echo "ROOTSYS not defined; trying ${nextsearchpath}."
  if [[ -d ${nextsearchpath} ]]; then
    export ROOTSYS=${nextsearchpath}
    echo "Setting ROOTSYS to ${ROOTSYS}."
  fi
fi
#  Try 'use root' (last resort)
if [[ -z ${ROOTSYS} ]]; then
  echo "ROOTSYS not defined; trying 'use root'..."
  use root
fi

#  Check and report failure to find ROOT installation
if [[ -z ${ROOTSYS} ]]; then
  echo "ERROR:  The ROOT environment variables could not be set."
  echo "        Please ensure ROOT is installed on the system,"
  echo "        set the ROOTSYS environment variables,"
  echo "        and re-run this program."
  exit 1
fi

# Check ROOT version
${QWANALYSIS}/SetupFiles/checkrootversion



###  Create the setup scripts.

cat > $local_path/SET_ME_UP.csh << END
#!/bin/csh
#
set analyzer_version = $QWANALYSIS

###
###  DO NOT CHANGE THE COMMANDS BELOW!!!
###
###  This script is automatically generated, all changes will be overwritten!
###

setenv QWANALYSIS \$analyzer_version

### Set MYSQL_INC_DIR and MYSQL_LIB_DIR
setenv MYSQL_INC_DIR $MYSQL_INC_DIR
setenv MYSQL_LIB_DIR $MYSQL_LIB_DIR

### Set MYSQLPP_INC_DIR and MYSQLPP_LIB_DIR
setenv MYSQLPP_INC_DIR $MYSQLPP_INC_DIR
setenv MYSQLPP_LIB_DIR $MYSQLPP_LIB_DIR

### Set ROOTSYS
setenv ROOTSYS $ROOTSYS

### Source the setup script
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
###  This script is automatically generated, all changes will be overwritten!
###

export QWANALYSIS=\$analyzerversion

### Set MYSQL_INC_DIR and MYSQL_LIB_DIR
export MYSQL_INC_DIR=$MYSQL_INC_DIR
export MYSQL_LIB_DIR=$MYSQL_LIB_DIR

### Set MYSQLPP_INC_DIR and MYSQLPP_LIB_DIR
export MYSQLPP_INC_DIR=$MYSQLPP_INC_DIR
export MYSQLPP_LIB_DIR=$MYSQLPP_LIB_DIR

### Set ROOTSYS
export ROOTSYS=$ROOTSYS

### Source the setup script
. \$QWANALYSIS/SetupFiles/.QwSetup.bash

unset analyzerversion

END

cd $orig_dir
