# Odd-Even Sort parallel with MPI in C++ and Python

The base implementation was taken from here:
http://cs.umw.edu/~finlayson/class/fall14/cpsc425/notes/18-sorting.html

We extend the base implemantion. One in C++. One in Python.

In both extensions, the nodes read the random values from a binary file.

The C++ implementation can also deal with non even proportion of nodes and count of values. 

## C++ Implementation:

Install MPICH: http://mpitutorial.com/tutorials/installing-mpich2/

1. Generating binary file with random values:

   Compile:
   ```
   g++ RandomNumberGenerator.cpp -o RandomNumberGenerator
   ```
   Run:
   ```
   ./RandomNumberGenerator <count_values_to_generate> <optinal_flag "y" if_output_on_console_is_desired>
   ```
   Example-Call:
   ```
   ./RandomNumberGenerator 100 y
   ```
   The generated result file have this format and will saved in the same directory, where this program will executed:
   ```
   <countNumbersToGenerate> numbers - <Current Date>.bin
   ```
   Example-Output:
   ```
   100 numbers - Wed Dez 13 21:42:25 2017.bin
   ```
2. Odd-Even Sort
   
   Compile:
   ```
   mpic++ OddEvenSort.cpp -o OddEvenSort
   ```
   Run on single machine:
   ```
   mpirun -n <count_nodes/proccesses> ./OddEvenSort "<numbers_file.bin>"
   ```
   Example-Call:
   ```
   mpirun -n 4 ./OddEvenSort "100 numbers - Wed Dez 13 21:42:25 2017.bin"
   ```
   On a cluster you have to fill your hostfile (/etc/hosts) and pass it with the -f flag like:
   ```
   mpirun -n 4 -f hosts ./OddEvenSort "100 numbers - Wed Dez 13 21:42:25 2017.bin"
   ```


## Python Implementation:

make sure Python3 is installed on your system

make sure numpy is installed on your system

for generating numbers:

```
python3 generator.py 128

```

for sorting :

```
mpiexec python3 sort.py

```
