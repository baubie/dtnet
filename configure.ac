AC_INIT(src/dtnet.cpp)

AM_INIT_AUTOMAKE(dtnet,2.0)

AC_PROG_CXX
AC_PROG_LIBTOOL

AC_CHECK_LIB([readline], [readline])
AC_CHECK_LIB([ncurses], [main])
AC_CHECK_LIB([rt], [clock_gettime])

AX_BOOST_BASE(1.37)
AX_BOOST_SERIALIZATION
AX_BOOST_IOSTREAMS
AX_BOOST_DATE_TIME
AX_BOOST_THREAD

AC_OUTPUT(Makefile src/Makefile src/lib/tinyxml/Makefile)
