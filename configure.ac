AC_INIT(src/dtnet.cpp)

AM_INIT_AUTOMAKE(dtnet,2.0)

AC_PROG_RANLIB

CXXFLAGS="$CXXFLAGS -O3"

CLI=true
GUI=false

AC_ARG_DISABLE(cli,
    [ --disable-cli    Disable the command line interface to dtnet ],
      CLI=false
     ])

AC_ARG_DISABLE(gui,
    [ --disable-gui    Disable the graphical user interface to dtnet ],
      GUI=false
     ])

AC_ARG_ENABLE(debug,
    [ --enable-debug    Enable debugging symbols ],
    [ CXXFLAGS="$CXXFLAGS -g"
      AC_DEFINE([DEBUG])
     ])

AC_ARG_ENABLE(text-archive,
    [ --enable-text-archive     Use text archiving mode to save simulations.  Uses more disk space but is required on Mac OSX ],
    [ AC_DEFINE([SERIALIZE_TEXT]) 
    ]) 

AM_CONDITIONAL([BUILD_CLI], [test x$CLI = xtrue])
AM_CONDITIONAL([BUILD_GUI], [test x$GUI = xtrue])

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

AX_BOOST_BASE(1.38)
AX_BOOST_PROGRAM_OPTIONS
AX_BOOST_FILESYSTEM
AX_BOOST_SERIALIZATION
AX_BOOST_IOSTREAMS
AX_BOOST_SYSTEM
AX_BOOST_THREAD
AX_BOOST_DATE_TIME

AC_OUTPUT(Makefile src/Makefile src/lib/tinyxml/Makefile)
