TARGET = ../../bin/sdlaunch

####### Compiler, tools and options

CXX           = g++
DEFINES       =
CFLAGS        = -pipe -O2 -fno-strict-aliasing -Wall -W -D_REENTRANT -fPIC $(DEFINES)
CXXFLAGS      = -pipe -O2 -fno-strict-aliasing -Wall -W -D_REENTRANT -fPIC $(DEFINES) -std=gnu++0x
LINK          = g++
LFLAGS        = -Wl,--hash-style=gnu -Wl,--as-needed -Wl,-O1
STRIP         = strip

BPO_INC       =
BPO_LIB       = -lboost_program_options

INCPATH = $(BPO_INC)
####### Build rules

all: $(TARGET)

launch.o: launch.cpp config.h itoa.h HostPort.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o launch.o launch.cpp

$(TARGET): launch.o
	mkdir -p ../../bin
	$(LINK) $(LFLAGS) -o $(TARGET) launch.o $(BPO_LIB) -pthread
	$(STRIP) $(TARGET)

clean:
	-rm -f *.o *~

distclean: clean
	-rm -f $(TARGET)
