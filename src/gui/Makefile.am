bin_PROGRAMS = gdtnet
gdtnet_SOURCES = gdtnet.cpp mainwindow.cpp project.cpp

gdtnet_CPPFLAGS = $(WX_CPPFLAGS)
gdtnet_CXXFLAGS = $(WX_CXXFLAGS_ONLY) $(BOOST_CPPFLAGS)
gdtnet_CFLAGS = $(WX_CFLAGS_ONLY)
gdtnet_LDADD = $(WX_LIBS)
gdtnet_LDFLAGS = -ldtnet $(BOOST_LDFLAGS)
gdtnet_LIBADD = $(BOOST_SERIALIZATION_LIB) 
