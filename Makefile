CPPFLAGS += -std=c++11 
LDFLAGS += -v 
LDFLAGS += -lstdc++

all: test
homie.o: homie.cpp
test.o: test.cpp

test: test.o homie.o

clean:
	rm -f *.o test

