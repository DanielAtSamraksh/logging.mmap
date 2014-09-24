
testfile = logtest

all: test

test: test.logfile test.logger test.example

test.logger: logfile.o logger.h test_logger.cpp 
	g++ test_logger.cpp logfile.o mkpath.o -o $@
	./$@ ${testfile}
	ls -l ${testfile}.*
	cmp ${testfile}.string.write ${testfile}.string.mmap
	cmp ${testfile}.ints.write ${testfile}.ints.mmap
	rm ${testfile}*.write ${testfile}*.mmap
	@echo $@ OK; echo

test.logfile: logfile.o
	g++ test_logfile.cpp logfile.o mkpath.o -o $@
	./$@ ${testfile}
	@# echo extra >> ${testfile}.write # test by inducing an error
	cmp ${testfile}.write ${testfile}.mmap
	rm ${testfile}.write ${testfile}.mmap
	@echo $@ OK; echo

mkpath.o: mkpath.h mkpath.cpp
	g++ -o $@ -c mkpath.cpp

logfile.o: mkpath.o logfile.cpp logfile.h
	g++ -o $@ -c logfile.cpp

test.example: example.cpp mkpath.o logfile.o logger.h
	g++ example.cpp logfile.o mkpath.o -o $@
	./$@ ${testfile}
	cmp ${testfile}.fancy ${testfile}.plain
	rm ${testfile}.fancy ${testfile}.plain
	@echo $@ OK; echo

clean: 
	rm -f *.o test.logfile test.logger test.example ${testfile}.*
