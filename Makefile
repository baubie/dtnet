CCFLAGS = -g -Wall -O3 -ffast-math -funroll-loops -mtune=nocona -ftree-vectorize
CXX = g++ $(CCFLAGS) 
LFLAGS = -lrt -L /home/aubiebn/local64/lib -lboost_thread-gcc43-mt -lboost_iostreams-gcc43-mt -lreadline -lcurses
SOURCES = src/dtnet.cpp src/net.cpp src/input.cpp src/trial.cpp src/population.cpp src/neuron.cpp src/parseargs.cpp src/lib/tinyxml/tinyxml.cpp src/lib/tinyxml/tinyxmlerror.cpp src/lib/tinyxml/tinyxmlparser.cpp src/lib/tinyxml/tinystr.cpp src/dtlang.cpp src/GLE.cpp src/simulation.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = dtnet

all: $(SOURCES) $(EXECUTABLE) 

$(EXECUTABLE): $(OBJECTS)
	${CXX} ${LFLAGS} ${OBJECTS} -o $@

.o:
	${CXX} $< -o $@

clean:
	rm -f dtnet src/*.o src/lib/tinyxml/*.o

docs:
	doxygen
