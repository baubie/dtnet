lib_LTLIBRARIES = libdtnet.la
libdtnet_la_SOURCES = libdtnet.cpp GLE.cpp net.cpp population.cpp neuron.cpp simulation.cpp trial.cpp input.cpp results.cpp range.cpp settings.cpp neuronfactory.cpp
libdtnet_la_LDFLAGS = -version-info 1:0:0
SUBDIRS = lib/tinyxml
LDADD = lib/tinyxml/libtinyxml.a models/libaEIF.a models/libPoisson.a $(BOOST_THREAD_LIB) $(BOOST_IOSTREAMS_LIB) $(BOOST_SERIALIZATION_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_FILESYSTEM_LIB) $(BOOST_PROGRAM_OPTIONS_LIB)

AM_CXXFLAGS = $(BOOST_CPPFLAGS)
AM_LDFLAGS = $(BOOST_LDFLAGS)
