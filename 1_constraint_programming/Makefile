CXX  = g++ -std=c++11
DIR  = /Library/Frameworks
LIBS = -lgecodedriver    -lgecodesearch  \
       -lgecodeminimodel -lgecodeint     \
       -lgecodekernel    -lgecodesupport

all: BWP


BWP.o: BWP.cpp
	$(CXX) -F$(DIR) -c BWP.cpp -g

BWP: BWP.o
	$(CXX) -F$(DIR) -framework gecode -o BWP BWP.o


