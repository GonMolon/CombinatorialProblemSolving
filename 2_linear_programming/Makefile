CPLEX_STUDIO_DIR = /Applications/CPLEX_Studio1210
SYSTEM           = x86-64_osx
LIBFORMAT        = static_pic
CPLEXDIR         = $(CPLEX_STUDIO_DIR)/cplex
CONCERTDIR       = $(CPLEX_STUDIO_DIR)/concert

################

CXX  = g++ -std=c++11

CCOPT = -m64 -O3 -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD
#CCOPT = -m64 -O3 -fPIC -fno-strict-aliasing -fexceptions -DIL_STD

CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

CCLNFLAGS = -L$(CPLEXLIBDIR) -lilocplex -lcplex -L$(CONCERTLIBDIR) -lconcert -lm -pthread -ldl

# -ldl needed for CPLEX version > 12.6


CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include

CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR)


all: BWP


BWP.o: BWP.cpp
	$(CXX) $(CCFLAGS) -c BWP.cpp -o BWP.o -g

BWP: BWP.o
	$(CXX) $(CCFLAGS) BWP.o -o BWP $(CCLNFLAGS)

