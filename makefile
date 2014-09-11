
# our compiler
CC=gcc

# compile arguments
CFLAGS+=-c -g -Wall

# linker flags
LDFLAGS+=

# libraries
LIBS+=-lncurses

# our source files
SOURCES=ex1.c

# a macro to define the objects from sources
OBJECTS=$(SOURCES:.c=.o)

# executable name
EXECUTABLE=testEx1

$(EXECUTABLE): $(OBJECTS)

	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

# a rule for generating object files given their c files
.c.o:

	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *s *o $(EXECUTABLE)

.PHONY: all clean
