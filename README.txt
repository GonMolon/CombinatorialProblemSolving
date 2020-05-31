%%%%%%%%%%%%%%%%%%%%
COMPILING
%%%%%%%%%%%%%%%%%%%%

The Makefiles are adapted to compile in MacOs. To compile for linux modify them accordingly.


%%%%%%%%%%%%%%%%%%%%
RUNNING
%%%%%%%%%%%%%%%%%%%%

To run all the instance with metrics and a timeout of 60 seconds for any deliverable:

python run.py path_to_deliverable

Where path_to_deliverable = {1_constraint_programming, 2_linear_programming, 3_sat}




To run a single instance, follow the example:

make && time ./BWP < ../instances/bwp_11_8_1.inp

To also run the checker over the solution found:

make && time ./BWP < ../instances/bwp_10_10_1.inp | ../checker
