%%%%%%%%%%%%%%%%%%%%
COMPILING
%%%%%%%%%%%%%%%%%%%%

The Makefile is adapted to compile in MacOs. To compile for linux modify it accordingly.


%%%%%%%%%%%%%%%%%%%%
RUNNING
%%%%%%%%%%%%%%%%%%%%

To run a single instance, follow the example:

make && time ./BWP < instances/bwp_11_8_1.inp

To also run the checker over the solution found:

make && time ./BWP < instances/bwp_10_10_1.inp | ./checker

To run all the instances with metrics and a timeout of 60 seconds:

python run.py

