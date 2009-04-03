bin_PROGRAMS = dtnet
dtnet_SOURCES = dtnet.cpp dtlang.cpp 
dtnet_LDADD = $(BOOST_PROGRAM_OPTIONS_LIB)
dtnet_CXXFLAGS = $(BOOST_CPPFLAGS)
dtnet_LDFLAGS = -L../libdtnet/.libs -ldtnet $(BOOST_LDFLAGS) $(CLI_LDFLAGS)
