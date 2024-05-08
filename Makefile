OPTS= -g -Wuninitialized -Wmaybe-uninitialized -Wall -Wshadow -Wcast-qual \
      -std=c++11 -Wextra -pedantic -Wno-unused-parameter

SOURCES=$(shell find ./src -name "*.cpp")

OBJECTS=$(SOURCES:./src/%.cpp=./build/$(notdir %).o)

all: dirs $(OBJECTS) midi2

dirs:
	mkdir -p build

tests:
	g++ $(OPTS) -I . -I ./include -o tests $(SOURCES) tests.cpp
	./tests

build/%.o: src/%.cpp
	g++ $(OPTS) -I . -I ./include \
		-o $@ -c $< 

midi2: $(OBJECTS)
	ar -rc build/libmidi2.a $(OBJECTS)

clean:
	rm -rf build
	rm tests
