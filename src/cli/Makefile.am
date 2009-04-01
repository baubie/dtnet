bin_PROGRAMS = dtnet
dtnet_SOURCES = dtnet.cpp dtlang.cpp 
LDADD = ../libdtnet/.libs/libdtnet.a $(BOOST_THREAD_LIB) $(BOOST_IOSTREAMS_LIB) $(BOOST_SERIALIZATION_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_FILESYSTEM_LIB) $(BOOST_PROGRAM_OPTIONS_LIB) ../libdtnet/lib/tinyxml/libtinyxml.a 

AM_CXXFLAGS = $(BOOST_CPPFLAGS)
AM_LDFLAGS = $(BOOST_LDFLAGS)
