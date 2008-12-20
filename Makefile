CCFLAGS = -g -Wall -O3 -ffast-math -funroll-loops -mtune=nocona -ftree-vectorize
CXX = g++ $(CCFLAGS) 
LFLAGS = -lrt -L /home/aubiebn/local64/lib -lboost_thread-gcc43-mt -lboost_iostreams-gcc43-mt -lreadline -lcurses
SOURCES = dtnet.cpp net.cpp input.cpp trial.cpp population.cpp neuron.cpp parseargs.cpp lib/tinyxml/tinyxml.cpp lib/tinyxml/tinyxmlerror.cpp lib/tinyxml/tinyxmlparser.cpp lib/tinyxml/tinystr.cpp dtlang.cpp gri++.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = dtnet

all: $(SOURCES) $(EXECUTABLE) 

$(EXECUTABLE): $(OBJECTS)
	${CXX} ${LFLAGS} ${OBJECTS} -o $@

.o:
	${CXX} $< -o $@

clean:
	rm -f dtnet *.o *.out lib/tinyxml/*.o
