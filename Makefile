
testfile = logtest

all: test

mkpath: mkpath.h mkpath.cpp
	g++ -D TEST_MKPATH mkpath.cpp -o test_mkpath

test_mkpath: mkpath test_mkpath.sh
	./test_mkpath.sh
	@echo $@ OK

logfile: logfile.h logfile.cpp mkpath.cpp
	g++ -D TEST_LOGFILE -o test_logfile logfile.cpp mkpath.cpp

test_logfile: logfile test_logfile.sh
	./test_logfile.sh
	@echo $@ OK

logger: logfile.h logfile.cpp mkpath.cpp logger.h logger.cpp
	g++ -D TEST_LOGGER -o test_logger mkpath.cpp logfile.cpp logger.cpp

test_logger: logger test_logger.sh
	./test_logfile.sh
	@echo $@ OK

log.h: mkpath.h mkpath.cpp logfile.h logfile.cpp logger.h Makefile
	cat mkpath.h logfile.h logger.h > $@
	@echo "$@ OK"

test_log.h: test_log
	@echo "$@ OK"

test_log: log.h test_log.cpp test_log.sh
	@echo "Testing $@"
	g++ test_log.cpp -o test_log
	./test_log.sh
	@echo "$@ OK"

test: log.h test.logfile test.logger test.example test.example2 Makefile

test.logger: logfile.o logger.h test_logger.cpp Makefile
	g++ test_logger.cpp logfile.o mkpath.o -o $@
	./$@ ${testfile}
	ls -l ${testfile}.*
	cmp ${testfile}.string.write ${testfile}.string.mmap
	cmp ${testfile}.ints.write ${testfile}.ints.mmap
	cmp ${testfile}.repr.write ${testfile}.repr.mmap
	/bin/echo -e 'struct my_repr_t {\n  int i;\n  char s[10];' |\
	  diff - ${testfile}.repr.mmap.repr
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

test.example2: example2.cpp log.h
	g++ example2.cpp  -o $@
	./$@ ${testfile}
	cmp ${testfile}.fancy ${testfile}.plain
	rm ${testfile}.fancy ${testfile}.plain
	@echo $@ OK; echo

clean: 
	rm -f *.o test.logfile test.logger test.example test.example2 ${testfile}.* *.out
