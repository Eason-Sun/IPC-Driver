# IPC-Driver

## Problem Definition:
This is a driver program in C++ that uses Inter-Process Communication (IPC) to link the output of the random input generator (rgen.cpp) to the input of the Python script (streetGraphGen.py), and the output of the Python script to the input of another C++ program (bfs.cpp).

# Build and Run:  
$ mkdir build && cd build  
$ cmake ../  
$ make install  
$ cd ./run/bin  
$ ./driver -s 5 -n 4 -l 5  

## About streetGraphGen.py:
Please check the link below:  
https://github.com/Eason-Sun/Optimization-of-security-cameras-using-vetex-cover

## About bfs.cpp:
Please check the link below:  
https://github.com/Eason-Sun/Breadth-First-Search

## About rgen.cpp:
The random input generator rgen generates random inputs of street specifications for the Python script. It takes four command-line arguments. All are optional.

 **-s k** - where k is an integer >= 2. The number of streets should be a random integer in [2; k]. If this option is not specied, you should use a default of k = 10; that is, the number of streets should be a random integer in [2; 10].

**-n k** - where k is an integer >= 1. The number of line-segments in each street should be a random integer in [1; k]. Default: k = 5.

**-l k** - where k is an integer >= 5. The process should wait a random number w seconds, where w is in [5; k] before generating the next (random) input. Default: k = 5.

**-c k** - where k is an integer >= 1. The process should generate (x; y) coordinates such that every x and y value is in the range [-k; k]. For example, if k = 15, all coordinate values should be integers between 􀀀15 and 15. Default: k = 20.

The program will generate a specification of streets in the format that your Python script. If it generates a line-segment that overlaps
with a line-segment (across all streets) generated earlier, the program will regenerate that line-segment. Similarly, there is no zero-length line segments. Also, the random generator goes into an infinite loop looking for a valid specification. I disallowed this from happening by limiting the number of tries. That is, if the random generator fails to generate a valid specification for a continuous A number of attempts, it will exit() with an error message reported on stderr. A reasonable A to adopt may be 25. The error message will be "Error: failed to generate valid input for 25 simultaneous attempts".

Before a new specification to the Python script, the generator will issue "r" commands to the Python script to remove all previous streets and replace them with the new specification of streets.

After generating the input, the generator will issue the "g" command. Thus, a typical interaction of the random generator is as follows:

(a) Issue enough r commands to clear any existing street database;  
(b) Issue a commands to add new streets satisfying the specification;  
(c) Issue a g command;  
(d) Wait for specified number of seconds and repeat.  

Note: /dev/urandom is used as the source of the random data.

## About driver.cpp:
The driver program has the overall control. You have at least three programs that run concurrently:  
(1) the random generator   
(2) the Python script  
(3) bfs.cpp that takes as input a graph-specification and computes shortest paths using Breadth First Search.  

Driver program will fork() two processes and exec() two of those programs, and then exec() the third (so it turns into a process that corresponds to the third program). It will set up all IPC appropriately beforehand. It will send normal output to stdout, error output to stderr, and take input from stdin. The only input it takes are "s" commands, that ask for a shortest path between vertices.


## Example:
**$ ./driver -s 5 -n 4 -l 5**  
**V 8** # Output: The randomly generated maximum number of vertices  
**E {<0,2>,<0,3>,<0,4>,<1,3>,<4,7>,<5,2>,<5,6>}** # Output: The randomly generated edges  
**s 2 4** # User input: If you'd like to know the shortest path from vertex 2 to vertex 4  
**2-0-4** # Output: The shortest path result  
