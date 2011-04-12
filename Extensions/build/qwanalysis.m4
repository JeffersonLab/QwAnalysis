dnl -*- mode: autoconf -*- 
dnl
dnl $Id: qwanalysis.m4, v0.1 Tuesday, April 12 17:03:18 EDT 2011 $
dnl $Author: Jeong Han Lee $
dnl $Date: Tuesday, April 12 17:03:39 EDT 2011 $
dnl
dnl Autoconf macro to check for Qweak analysis on the system
dnl most things copied from root.m4
dnl

dnl the macro defines the following substitution variables
dnl 
dnl    QWEAKLIBDIR  full path to qweak-config
dnl    QWEAKINCDIR  Where the **ONLY** Qweak header files
dnl    QWEAKCFLAGS  Where the **
dnl    QWEAKLIBS    QWEAK all libraries
dnl    
dnl   Jeong Han Lee <jhlee@jlab.org>

AC_DEFUN([QWEAK_PATH],
[
  AC_ARG_WITH([qwanalysis],
              [AC_HELP_STRING([--with-qwanalysis],
			      [top of the Qweak installation directory])],
    			      [user_qwanalysis=$withval],
			      [user_qwanalysis="none"])
  if test ! x"$qwanalysis" = xnone; then
     qweakbin="$user_qwanalysis/bin"
  elif test ! x"$QWANALYSIS" = x ; then 
    qweakbin="$QWANALYSIS/bin"
  else 
   qweakbin=$PATH
  fi

  AC_PATH_PROG(QWEAKCONF, qweak-config , no, $qweakbin)
   if test ! x"$QWEAKCONF" = "xno"; then
      QWEAKLIBDIR=`$QWEAKCONF --lib`	
      QWEAKINCDIR=`$QWEAKCONF --incdir`
      QWEAKCFLAGS=`$QWEAKCONF --cpflags`		      
      QWEAKLIBS=`$QWEAKCONF --libs`
   else
      no_qweak="yes"
   fi

  AC_SUBST(QWEAKLIBDIR)
  AC_SUBST(QWEAKINCDIR)
  AC_SUBST(QWEAKCFLAGS)
  AC_SUBST(QWEAKLIBS)

])

#
# EOF
#
