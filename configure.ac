dnl Process this file with autoconf to produce a configure script.
AC_INIT([TOPCOM],[1.1.2])
AC_CONFIG_SRCDIR(.)

dnl Init automake.
AM_INIT_AUTOMAKE

dnl set the TOPCOM root directory:
AC_SUBST([TOPCOMROOT], [`pwd`])

dnl set some compilation switches consistently:
AC_SUBST([AM_CPPFLAGS], ["-DSTL_SYMMETRIES -DSTL_CONTAINERS -DSTL_FLIPS -DSTL_CHIROTOPE -DGMPRATIONAL -DPERMLIB_DOMAIN_INT -DUSE_LEXORDER]")
AC_SUBST([AM_CXXFLAGS], [" -fvisibility=hidden -fvisibility-inlines-hidden -pthread -std=c++17"])
AC_SUBST([AM_LDFLAGS], ["-L../external/lib]")

dnl backup the user variable LDFLAGS:
dnl AC_SUBST([LDFLAGS_backup], [${LDFLAGS}])

dnl add local libraries to LDFLAGS so that AC_CHECK_LIB will find them:
dnl AC_SUBST([LDFLAGS], ["-L${TOPCOMROOT}/external/lib"])

dnl Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_CXX
AC_PROG_RANLIB

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST


dnl Check for libraries:
dnl Make libgmp if requested:
AC_LANG_PUSH(C++)
AC_CHECK_HEADERS([gmpxx.h])
AC_LANG_POP
AM_CONDITIONAL(USE_LOCAL_GMP, [test "x$ac_cv_header_gmpxx_h" = "xno"])
if test "x$ac_cv_header_gmpxx_h" = "xyes"; then
  AC_MSG_NOTICE([system libgmp is used])
else
  AC_MSG_NOTICE([gmpxx.h not found on system - building gmp locally ...])
  make -C external gmp
  AC_MSG_NOTICE([... done])
fi

dnl Make libcddgmp:
make -C external cdd

dnl Check for requests for third-party packages:
dnl Check for qsopt_ex:
AC_MSG_CHECKING(whether QSOpt_ex support was enabled)
AC_ARG_ENABLE([qsoptex],
  AS_HELP_STRING([--enable-qsoptex],
                 [use QSOpt_ex as an LP solver for regularity checks (requires QSOpt_ex includes in ${TOPCOM}/external/include and libqsopt_ex.a in ${TOPCOM}/external/lib (built automatically during configure)])
)
AM_CONDITIONAL(USE_QSOPTEX, [test "x$enable_qsoptex" = "xyes"])
if test "x$enable_qsoptex" = "xyes"; then
  AC_MSG_RESULT(yes)
  AC_DEFINE(HAVE_LIBQSOPTEX)

  dnl Check for compression libraries (qsopt_ex uses them if they are present):
  AC_CHECK_LIB([z], [gzopen], [], AC_MSG_NOTICE([compiling without libz]))
  AC_CHECK_LIB([bz2], [BZ2_bzopen], [], AC_MSG_NOTICE([compiling without libbz2]))

  dnl Make libqsopt_ex:
  make -C external qso
else
  AC_MSG_RESULT(no)
fi

dnl Check for soplex:
AC_MSG_CHECKING(whether soplex support was enabled)
AC_ARG_ENABLE([soplex],
  AS_HELP_STRING([--enable-soplex],
                 [use soplex as an LP solver for regularity checks (requires soplex includes and boost in ${TOPCOM}/external/include and libsoplex.a in ${TOPCOM}/external/lib)])
)
if test "x$enable_soplex" = "xyes"; then  
  AC_MSG_RESULT(yes)

  dnl Check for compression library (soplex needs it):
  AC_CHECK_LIB([z], [gzopen], [LIBS="-lsoplex $LIBS"; AC_DEFINE(HAVE_LIBSOPLEX)], AC_MSG_NOTICE([libz missing => soplex not enabled]))
else
  AC_MSG_RESULT(no)
fi
AM_CONDITIONAL(USE_SOPLEX, [test "x$ac_cv_lib_z_gzopen" = "xyes" -a "x$enable_soplex" = "xyes"])

dnl Check for Permlib:
dnl AC_MSG_CHECKING(whether permlib support was enabled)
dnl AC_ARG_ENABLE([permlib],
dnl   AS_HELP_STRING([--enable-permlib],
dnl                  [support permlib for symmetry checks (requires permlib-includes and boost in ${TOPCOM}/external/include)])
dnl )
dnl AM_CONDITIONAL(USE_PERMLIB, [test "x$enable_permlib" = "xyes"])
dnl if test "x$enable_permlib" = "xyes"; then
dnl   AC_DEFINE(HAVE_LIBPERMLIB)
dnl   AC_MSG_RESULT(yes)
dnl else
dnl   AC_MSG_RESULT(no)
dnl fi

dnl restore the user variable LDLFAGS:
dnl AC_SUBST([LDFLAGS], [${LDFLAGS_backup}])

AC_CONFIG_FILES([wrap-gmp-gmpxx/Makefile lib-src/Makefile lib-src-reg/Makefile src/Makefile src-reg/Makefile examples/Makefile share/Makefile Makefile])
AC_OUTPUT
