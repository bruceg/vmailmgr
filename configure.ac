AC_INIT([vmailmgr], [0.97.1])
AC_CONFIG_SRCDIR([lib/mergelib.sh])
AM_INIT_AUTOMAKE

AH_TEMPLATE(USE_CRYPT, [Use only crypt(3) for authentication instead of the built-in MD5 password hashing.])
AH_TEMPLATE(USE_KRB5, [Use Kerberos5 for authentication.])
AH_TEMPLATE(VPWTABLE_CDB, [Use "cdb" database for storage of virtual password tables.])
AH_TEMPLATE(VPWTABLE_GDBM, [Use "gdbm" database for storage of virtual password tables.])

USE_CRYPT=no
AC_ARG_ENABLE(crypt,
[  --enable-crypt          Use only crypt(3) for authentication instead
                           of the built-in MD5 password hashing.],
USE_CRYPT=$enableval)
test "$USE_CRYPT" != no && AC_DEFINE(USE_CRYPT)

AC_ARG_WITH(krb5,
[  --with-krb5             Use Kerberos5 for authentication.],
[test x$withval = xyes && withval=/usr/krb5
 LIBS="$LIBS -L$withval/lib -lkrb5 -lcom_err -lcrypto"
 CFLAGS="$CFLAGS -I${withval}/include"
 CXXFLAGS="$CXXFLAGS -I${withval}/include"
 AC_DEFINE(USE_KRB5)])
AM_CONDITIONAL(KRB5, test x$with_krb5 != x -a x$with_krb5 != xno)

AC_ARG_WITH(vpwtable,
[  --with-vpwtable=type    Use "type" database for storage of virtual
                           password tables.  Defaults to "cdb".],
[case "${withval}" in
 gdbm)	AC_DEFINE(VPWTABLE_GDBM)
	dbtype=gdbm
	LIBS="$LIBS -lgdbm";;
 *)    AC_MSG_ERROR(bad value ${withval} for --with-vpwtable);;
 esac],[AC_DEFINE(VPWTABLE_CDB)
	dbtype=cdb])

AM_CONDITIONAL(VPWTABLE_CDB, test $dbtype = cdb)
AM_CONDITIONAL(VPWTABLE_GDBM, test $dbtype = gdbm)

AM_CONDITIONAL(FDBUF_NO_MYSTRING, false)

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CXX
CXX_NO_RTTI
CXX_NO_EXCEPTIONS

CFLAGS="$CFLAGS -Wall"
CXXFLAGS="$CXXFLAGS -Wall"

AC_PROG_INSTALL
AC_PROG_RANLIB
AC_PATH_PROG(STRIP, strip)
AC_PATH_PROG(RM, rm)
AC_PATH_PROG(MKDIR, mkdir)
AC_PATH_PROG(LN, ln)

AC_PATH_PROG(PYTHON, python2)
AC_CACHE_CHECK(Python library path, local_cv_python_lib_dir,
[[if test -x "$PYTHON"; then
  local_cv_python_lib_dir=`$PYTHON -c "import sys;print sys.path[1]"`
else
  local_cv_python_lib_dir=NONE
fi]])
pythonlibdir="$local_cv_python_lib_dir/vmailmgr"
AC_SUBST(pythonlibdir)
AM_CONDITIONAL(WANTPYTHON, test x$local_cv_python_lib_dir != xNONE)

phpdir="/home/httpd/php"
AC_SUBST(phpdir)

AC_PATH_PROG(W3M, w3m)
AC_PATH_PROG(LYNX, lynx)
if test "x$W3M" != x; then
  echo Using w3m to format html pages.
  HTML2TXT="w3m -dump"
elif test "x$LYNX" != x; then
  echo Using lynx to format html pages.
  HTML2TXT="lynx -dump -nolist"
else
  echo "Can't find either w3m or lynx, building the plain text"
  echo "documentation files will fail."
  HTML2TXT="false"
fi
AC_SUBST(HTML2TXT)

TODAY=`date +%Y-%m-%d`
POD2MAN="pod2man --release='vmailmgr ${VERSION}' --center='VMailMgr Tools Documentation' --date='${TODAY}'"
POD2HTML="pod2html --noindex"
SUFFIXES="$SUFFIXES .1 .5 .7 .8 .pod .html .texi .txt .ps .fig .sgml .py .pyc .pyo"
AC_SUBST(POD2MAN)
AC_SUBST(POD2HTML)
AC_SUBST(SUFFIXES)

dnl Checks for libraries.
AC_CHECK_LIB(crypt, crypt)
AC_CHECK_LIB(inet, socket)
AC_CHECK_LIB(socket, socket)
AC_CHECK_LIB(xnet, socket)

dnl Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_HEADER_TIME
AC_CHECK_HEADERS(fcntl.h sys/time.h unistd.h shadow.h crypt.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_UID_T
AC_TYPE_SIGNAL
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T

dnl Checks for library functions.
AC_CHECK_FUNCS(gethostname gettimeofday mkdir putenv rmdir socket)
AC_CHECK_FUNCS(srandom random crypt strcasestr)

cgidir='${prefix}/cgi-bin'
AC_SUBST(cgidir)

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES(Makefile
	cgi/Makefile
	authenticate/Makefile
	commands/Makefile
	daemon/Makefile
	doc/Makefile
	lib/Makefile
	lib/cdb++/Makefile
	lib/cgi/Makefile
	lib/cli++/Makefile
	lib/config/Makefile
	lib/courier-authlib/Makefile
	lib/fdbuf/Makefile
	lib/misc/Makefile
	lib/mystring/Makefile
	lib/vdomain/Makefile
	lib/vpwentry/Makefile
	lib/vpwtable/Makefile
	scripts/Makefile
	php/Makefile
	python/Makefile)
AC_OUTPUT
