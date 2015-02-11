# Building

To build the test, run make

# Running

To run the tests:

  -e <int>: Maximum number of elements  
  -t <int>: Number of GPU threads to use  
  -p <int>: Number of GPU threads per workgroup  
  -i <int>: Number of iterations to run each test  
  -n: output to file (data.csv) instead of the terminal  
 
Example:
  ./test -e 1048576 -t 1 -p 1 -i 100 -n

The test will use arrays from 256 elements to 1048576  elements. 
Each test runs 100 times through 1024 steps of the array.

More details can be found in main.cpp.

main.cpp is the main CPU source. The GPU kernel is in kernel.cl.
