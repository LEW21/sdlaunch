DESTDIR = /usr/local

all: bin/sdlaunch

bin/sdlaunch:
	cd src/sdlaunch && make

clean:
	-find . -name *~ -exec rm -f {} \;
	cd src && make clean

distclean: clean
	cd src && make distclean
	rm -rf bin/ lib/

install: bin/sdlaunch
	mkdir -p $(DESTDIR)/bin
	cp bin/sdlaunch $(DESTDIR)/bin

uninstall:
	rm $(DESTDIR)/bin/sdlaunch
