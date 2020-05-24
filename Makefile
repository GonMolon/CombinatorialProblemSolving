CXX  = g++ -std=c++11
DIR  = /Library/Frameworks
LIBS = -lgecodedriver    -lgecodesearch  \
       -lgecodeminimodel -lgecodeint     \
       -lgecodekernel    -lgecodesupport


all: BWP checker
       	
BWP: BWP.cpp
	$(CXX) -F$(DIR) -c BWP.cpp
	$(CXX) -F$(DIR) -framework gecode -o BWP BWP.o


checker: checker.cc
	$(CXX) -o checker checker.cc


