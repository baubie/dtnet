bin_PROGRAMS = qdtnet
qdtnet_SOURCES = main.cpp mainwindow.cpp 
qdtnet_LDADD = $(BOOST_SYSTEM_LIB)
qdtnet_CXXFLAGS = $(QT_CXXFLAGS) $(BOOST_CPPFLAGS)
qdtnet_LDFLAGS = -L../libdtnet/.libs -ldtnet $(BOOST_LDFLAGS) $(QT_LIBS)
