SUBDIRS = lib/tinyxml

lib_LTLIBRARIES = libdtnet.la
libdtnet_la_SOURCES = libdtnet.cpp GLE.cpp net.cpp population.cpp neuron.cpp simulation.cpp trial.cpp input.cpp results.cpp range.cpp settings.cpp neuronfactory.cpp
libdtnet_la_LDFLAGS = -no-undefined -version-info 1:0:0 $(BOOST_LDFLAGS)
libdtnet_la_LIBADD = lib/tinyxml/libtinyxml.la $(BOOST_THREAD_LIB) $(BOOST_IOSTREAMS_LIB) $(BOOST_SERIALIZATION_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_FILESYSTEM_LIB) $(BOOST_PROGRAM_OPTIONS_LIB)
libdtnet_la_CXXFLAGS = $(BOOST_CPPFLAGS)
