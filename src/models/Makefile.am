lib_LTLIBRARIES = libdtnet_aeif.la libdtnet_poisson.la
libdtnet_aeif_la_SOURCES = aEIF.cpp
libdtnet_poisson_la_SOURCES = poisson.cpp

libdtnet_aeif_la_LDFLAGS = -version-info 1:0:0
libdtnet_poisson_la_LDFLAGS = -version-info 1:0:0

LDADD = ../libdtnet/.libs/libdtnet.a

INCLUDES = -I ../libdtnet

AM_CXXFLAGS = $(BOOST_CPPFLAGS)
AM_LDFLAGS =  $(BOOST_LDFLAGS)
