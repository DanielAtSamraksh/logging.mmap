

all: test

test: test.logging
	./test.logging

test.logging: mkpath.o logging.cpp logging.h
	g++ -DTEST_LOGGING logging.cpp mkpath.o -o test.logging

mkpath.o:: mkpath.h mkpath.cpp
	g++ -o mkpath.o mkpath.cpp
