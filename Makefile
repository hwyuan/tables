CC=g++
CFLAGS=-c -Wall 
LDFLAGS=
LIBS += -lboost_system -lndn-cpp-dev
SOURCES=city.cpp name-tree-entry.cpp name-tree.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=npht

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o npht
