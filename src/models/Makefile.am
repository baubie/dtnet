lib_LTLIBRARIES = libdtnet_aeif.la libdtnet_poisson.la libdtnet_hh.la

libdtnet_aeif_la_SOURCES = aEIF.cpp
libdtnet_poisson_la_SOURCES = poisson.cpp
libdtnet_hh_la_SOURCES = HH.cpp

libdtnet_aeif_la_LDFLAGS = -version-info 1:0:0 $(BOOST_LDFLAGS)
libdtnet_poisson_la_LDFLAGS = -version-info 1:0:0 $(BOOST_LDFLAGS)
libdtnet_hh_la_LDFLAGS = -version-info 1:0:0 $(BOOST_LDFLAGS) -L/opt/intel/cmkl/10.1.2.024/lib/em64t -lmkl_sequential -lmkl_core

libdtnet_aeif_la_CXXFLAGS = $(BOOST_CPPFLAGS)
libdtnet_poisson_la_CXXFLAGS = $(BOOST_CPPFLAGS)
libdtnet_hh_la_CXXFLAGS = $(BOOST_CPPFLAGS)

