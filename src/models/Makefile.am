lib_LTLIBRARIES = libdtnet_aeif.la libdtnet_poisson.la

libdtnet_aeif_la_SOURCES = aEIF.cpp
libdtnet_poisson_la_SOURCES = poisson.cpp

libdtnet_aeif_la_LDFLAGS = -version-info 1:0:0 $(BOOST_LDFLAGS)
libdtnet_poisson_la_LDFLAGS = -version-info 1:0:0 $(BOOST_LDFLAGS)

libdtnet_aeif_la_CXXFLAGS = $(BOOST_CPPFLAGS)
libdtnet_poisson_la_CXXFLAGS = $(BOOST_CPPFLAGS)

#libdtnet_aeif_la_LDADD = ../libdtnet/.libs/libdtnet.a $(BOOST_SERIALIZATION_LIB)
#libdtnet_poisson_la_LDADD = ../libdtnet/.libs/libdtnet.a $(BOOST_SERIALIZATION_LIB)
