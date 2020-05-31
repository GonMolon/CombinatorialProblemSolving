CXX  = g++ -std=c++11
DIR  = /Library/Frameworks
LIBS = -lgecodedriver    -lgecodesearch  \
       -lgecodeminimodel -lgecodeint     \
       -lgecodekernel    -lgecodesupport

       	
BWP: BWP.cpp
	$(CXX) -F$(DIR) -c BWP.cpp -g
	$(CXX) -F$(DIR) -framework gecode -o BWP BWP.o


