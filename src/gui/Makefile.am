bin_PROGRAMS = gdtnet
gdtnet_SOURCES = gdtnet.cpp lib/mathplot/mathplot.cpp

gdtnet_CPPFLAGS = $(WX_CPPFLAGS)
gdtnet_CXXFLAGS = $(WX_CXXFLAGS_ONLY) $(BOOST_CPPFLAGS)
gdtnet_CFLAGS = $(WX_CFLAGS_ONLY)
gdtnet_LDADD = $(WX_LIBS)
gdtnet_LDFLAGS = -L../libdtnet/.libs -ldtnet $(BOOST_LDFLAGS)
