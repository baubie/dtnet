SUBDIRS = lib/tinyxml

lib_LTLIBRARIES = libdtnet.la
libdtnet_la_SOURCES = libdtnet.cpp GLE.cpp net.cpp population.cpp neuron.cpp simulation.cpp trial.cpp input.cpp results.cpp range.cpp settings.cpp neuronfactory.cpp
libdtnet_la_LDFLAGS = -no-undefined -version-info 1:0:0 $(BOOST_LDFLAGS) $(BOOST_THREAD_LDFLAGS) $(BOOST_IOSTREAMS_LDFLAGS) $(BOOST_SERIALIZATION_LDFLAGS) $(BOOST_SYSTEM_LDFLAGS) $(BOOST_FILESYSTEM_LDFLAGS) $(BOOST_PROGRAM_OPTIONS_LDFLAGS)

libdtnet_la_LIBADD = lib/tinyxml/libtinyxml.la $(BOOST_THREAD_LIBS) $(BOOST_IOSTREAMS_LIBS) $(BOOST_SERIALIZATION_LIBS) $(BOOST_SYSTEM_LIBS) $(BOOST_FILESYSTEM_LIBS) $(BOOST_PROGRAM_OPTIONS_LIBS) 
libdtnet_la_CXXFLAGS = $(BOOST_CPPFLAGS) 
