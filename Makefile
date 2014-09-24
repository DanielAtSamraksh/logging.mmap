
testfile = logtest

all: test

test: test.logger

test.logger: logger 
	./$@ ${testfile}
	@# echo extra >> ${testfile}.write # test by inducing an error
	ls -l ${testfile}.*
	cmp ${testfile}.string.write ${testfile}.string.mmap
	cmp ${testfile}.ints.write ${testfile}.ints.mmap
	rm ${testfile}*.write ${testfile}*.mmap
	@echo $@ OK

test.logfile: logfile
	./$@ ${testfile}
	@# echo extra >> ${testfile}.write # test by inducing an error
	cmp ${testfile}.write ${testfile}.mmap
	rm ${testfile}.write ${testfile}.mmap
	@echo $@ OK



logfile: mkpath.o logfile.cpp logfile.h
	g++ -DTEST_LOGFILE logfile.cpp mkpath.o -o test.logfile

logger: mkpath.o logfile.o logger.cpp logger.h
	g++ -DTEST_LOGGER logger.cpp logfile.o mkpath.o -o test.logger

mkpath.o: mkpath.h mkpath.cpp
	g++ -o mkpath.o -c mkpath.cpp

logfile.o: logfile.cpp logfile.h
	g++ -o logfile.o -c logfile.cpp

clean: 
	rm -f *.o test.logfile test.logger ${testfile}.*
