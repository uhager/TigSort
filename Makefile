## TigSort Makefile
## author: Ulrike Hager
## Make sure that ROOTSYS and ROOTANA point to your ROOT and ROOTANA install paths, respectively.

ROOT_INCLUDES += -DHAVE_ROOT `root-config --cflags`

ROOTLIBS  = -L$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --libs)  -lXMLParser -lThread
ROOTGLIBS = -L$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --glibs) -lXMLParser -lThread
#RPATH    += -Wl,-rpath,$(ROOTSYS)/lib
CXXFLAGS += -DHAVE_ROOT $(shell $(ROOTSYS)/bin/root-config --cflags) -I.
OBJS = TigTree.o TigEvent.o TigDetector.o TigWaveform.o TigTimer.o TigUnpack.o TigSort.o TigManager.o TigScaler.o  

ifdef MIDASSYS
MIDASLIBS = $(MIDASSYS)/linux/lib/libmidas.a -lutil -lrt
CXXFLAGS += -DHAVE_MIDAS -DOS_LINUX -Dextname -I$(MIDASSYS)/include
endif

ROOTANA_INCLUDES = `root-config --cflags` -I${ROOTANA}/include/
ROOTANA_LIBS =  -L${ROOTANA} ${ROOTANA}/lib/librootana.a 

all: $(OBJS) tigsort

tigsort: $(OBJS)
	$(CXX) $(CXXFLAGS)  $(OBJS) $(ROOTANA_LIBS) $(MIDASLIBS)  $(ROOTLIBS) $(ROOTGLIBS) -lz -o $@ 
#	$(CXX) -o $@ $(CXXFLAGS) $^ $(MIDASLIBS)  $(ROOTANA_INCLUDES) $(ROOTANA_LIBS) $(ROOTGLIBS) $(ROOT_INCLUDES) $(ROOT_LIBS)  $(#RPATH) 

%.o: %.cxx
	$(CXX) $(CXXFLAGS) $(ROOTANA_INCLUDES) -o $@ -c $<

clean:
	rm -f *.o tigsort

