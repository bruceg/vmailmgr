dnl TRY_CXX_FLAG(FLAG,[ACTION-IF-FOUND[,ACTION-IF-NOT-FOUND]])
AC_DEFUN(TRY_CXX_FLAG,
[echo >conftest.cc
if ${CXX-g++} ${CXXFLAGS} -c [$1] conftest.cc >/dev/null 2>&1; then
  ifelse([$2], , :, [rm -f conftest*
  $2])
else
  ifelse([$3], , :, [rm -f conftest*
  $3])
fi
rm -f conftest*])

AC_DEFUN(CXX_NO_RTTI,
[AC_CACHE_CHECK(whether ${CXX-g++} accepts -fno-rtti,
	local_cv_flag_NO_RTTI,
	TRY_CXX_FLAG(-fno-rtti,
		local_cv_flag_NO_RTTI=yes,
		local_cv_flag_NO_RTTI=no))
test "$local_cv_flag_NO_RTTI" = yes && CXXFLAGS="$CXXFLAGS -fno-rtti"
])

AC_DEFUN(CXX_NO_EXCEPTIONS,
[AC_CACHE_CHECK(whether ${CXX-g++} accepts -fno-exceptions,
	local_cv_flag_NO_EXCEPTIONS,
	TRY_CXX_FLAG(-fno-exceptions,
		local_cv_flag_NO_EXCEPTIONS=yes,
		local_cv_flag_NO_EXCEPTIONS=no))
test "$local_cv_flag_NO_EXCEPTIONS" = yes && CXXFLAGS="$CXXFLAGS -fno-exceptions"
])

