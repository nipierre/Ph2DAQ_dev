Objs                    = RegManager.o GLIBInterface.o CbcInterface.o Exception.o Utilities.o
CC              = gcc
CXX             = g++
CCFlags         = -g -O1 -Wall -fPIC `root-config --cflags`
DevFlags                   = -D__CBCDAQ_DEV__
#DevFlags		=

IncludeDirs     =  /opt/cactus/include /opt/xdaq/include ../

IncludePaths            = $(IncludeDirs:%=-I%)

%.o: %.cc %.h
	$(CXX) $(DevFlags) $(CCFlags) $(UserCCFlags) $(CCDefines) $(IncludePaths) -c -o $@ $<


all: $(Objs) ../HWDescription/Definition.h
	$(CC) -shared -o libPh2_Interface.so $(Objs)
	mv libPh2_Interface.so ../lib

clean:
	rm -f *.o
