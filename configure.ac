AC_INIT(src/dtnet.cpp)

AM_INIT_AUTOMAKE(dtnet,2.0)

AC_PROG_CXX
AC_PROG_LIBTOOL


add_ldflags=""
READLINE_lib_check=""
AC_ARG_WITH(readline, 
    [ --with-readline=<path>    Where the readline library is located ], 
    [ READLINE_lib_check="$withval/lib"])
if test "$READLINE_lib_check" != ""
then
    add_ldflags="-L$READLINE_lib_check -lncurses"
fi


LDFLAGS="$LDFLAGS $add_ldflags"

AC_CHECK_LIB([readline], [readline])
AC_CHECK_LIB([ncurses], [main])
AC_CHECK_LIB([rt], [clock_gettime])

AX_BOOST_BASE(1.37)
AX_BOOST_SERIALIZATION
AX_BOOST_IOSTREAMS
AX_BOOST_DATE_TIME
AX_BOOST_THREAD

AC_OUTPUT(Makefile src/Makefile src/lib/tinyxml/Makefile)
