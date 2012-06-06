all: ../bin/sdlaunch

../bin/sdlaunch:
	cd sdlaunch && make

clean:
	-rm -f *.o *~
	cd sdlaunch && make clean

distclean: clean
	-rm -f ../bin ../lib
	cd sdlaunch && make distclean
