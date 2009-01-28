bin_PROGRAMS = dtnet
dtnet_SOURCES = dtnet.cpp dtlang.cpp GLE.cpp net.cpp population.cpp neuron.cpp simulation.cpp trial.cpp input.cpp parseargs.cpp results.cpp neuronparams.cpp range.cpp
SUBDIRS = lib/tinyxml
LDADD = lib/tinyxml/libtinyxml.a $(BOOST_THREAD_LIB) $(BOOST_IOSTREAMS_LIB) $(BOOST_SERIALIZATION_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_FILESYSTEM_LIB)

AM_CXXFLAGS = $(BOOST_CPPFLAGS)
AM_LDFLAGS = $(BOOST_LDFLAGS)
