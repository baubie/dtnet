bin_PROGRAMS = gdtnet
gdtnet_SOURCES = gdtnet.cpp mainwindow.cpp project.cpp network.cpp

gdtnet_CPPFLAGS = $(WX_CPPFLAGS)
gdtnet_CXXFLAGS = $(WX_CXXFLAGS_ONLY) $(BOOST_CPPFLAGS)
gdtnet_CFLAGS = $(WX_CFLAGS_ONLY)
gdtnet_LDADD = $(WX_LIBS) $(BOOST_SERIALIZATION_LIB)
gdtnet_LDFLAGS = -L../libdtnet/.libs -ldtnet $(BOOST_LDFLAGS)
