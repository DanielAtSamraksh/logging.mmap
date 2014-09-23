
testfile = logtest

all: test

test: test.logging
	./test.logging ${testfile}
	@# echo extra >> ${testfile}.write # test by inducing an error
	cmp ${testfile}.write ${testfile}.mmap
	rm ${testfile}.write ${testfile}.mmap
	@echo OK

test.logging: mkpath.o logging.cpp logging.h
	g++ -DTEST_LOGGING logging.cpp mkpath.o -o test.logging

mkpath.o: mkpath.h mkpath.cpp
	g++ -o mkpath.o -c mkpath.cpp

clean: 
	-rm *.o test.logging ${testfile}.*
