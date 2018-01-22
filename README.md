# Odd-Even Sort parallel with MPI in C++ and Python

The base implementation was taken from [here.](http://cs.umw.edu/~finlayson/class/fall14/cpsc425/notes/18-sorting.html)

We extend and improved the base implementation. One in [C++](#c++_header). One in [Python](#python_header).

In both extensions, the proccesses read the random values from a binary file.

The C++ implementation can also deal with non even proportion of proccesses and count of values.

The [result-table](#header_table_c++) with the C++ implementation.

The [result-table](#header_table_python) with the python implementation (processing).

<a name="c++_header"></a>
## C++ Implementation:

1. Install MPICH2: http://mpitutorial.com/tutorials/installing-mpich2/

2. Generating binary file with random values:

   Compile:
   ```
   g++ RandomNumberGenerator.cpp -o RandomNumberGenerator -std=gnu++0x
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
3. Execute the Odd-Even Sort
   
   Compile:
   ```
   mpic++ OddEvenSort.cpp -o OddEvenSort
   ```
   Run on single machine:
   ```
   mpirun -np <count_proccesses> ./OddEvenSort "<numbers_file.bin>"
   ```
   Example-Call:
   ```
   mpirun -np 4 ./OddEvenSort "100 numbers - Wed Dez 13 21:42:25 2017.bin"
   ```
   On a cluster you have to fill your hostfile (/etc/hosts) and pass it with the -f flag like:
   ```
   mpirun -np 4 -f hosts ./OddEvenSort "100 numbers - Wed Dez 13 21:42:25 2017.bin"
   ```
<a name="header_table_c++"></a>
### Result table C++

Time in seconds.

| Count Processes: | 2     | 4       | 10     | 100    | 200   | 300    | 400     | 500    |
|------------------|-------|---------|--------|--------|-------|--------|---------|--------|
| Count Values     |       |         |        |        |       |        |         |        |
| 509              | 0,860 | 0,890   | 1,092  | 3,032  | 6,355 | 8,057  | 12,476  | 13,909 |
| 5.117            | 0,910 | 0,960   | 1,092  | 7,174  | 6,556 | 10,984 | 11,779  | 13,161 |
| 51.197           | 1,896 | 1,707   | 1,510  | 3,241  | 5,737 | 8,762  | 10,763  | 13,515 |
| 511.997          | ?     | 243,636 | 47,941 | 10,196 | 9,776 | 13,692 | 15,110  | 19,445 |
| 5.119.997        | ?     | ?       | ?      | ?      | ?     | ?      | 128,571 | 113,09 |

<a name="python_header"></a>
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

<a name="header_table_python"></a>
### Result table Python

Time in seconds.

| Count Processes: | 4     | 8       | 16     | 32     | 64    | 128    | 256     |
|------------------|-------|---------|--------|--------|-------|--------|---------|
| Count Values     |       |         |        |        |       |        |         |
| 128	   	       | 1.963 | 1.853	 | 2.042  | 2.322  | 3.110 | 4.663  | N/A 	 |
| 4096	          | 1.855 | 1.921	 | 2.022  | 2.370  | 3.093 | 4.732  | 8.909   |
| 131072	          | 2.066 | 2.025	 | 2.344  | 2.698  | 3.633 | 4.840  | 8.478   |
| 4194304	       | 5.353 | 5.722	 | 6.225  | 6.654  | 7.312 | 9.851  | 13.817  | 
| 16777216         | 18.66 | 18.513	 | 19.546 | 22.012 | 22.13 | 24.340 | 29.714  |
| 33554432         | N/A   | N/A	    | 52.181 | 41.715 | 39.72 | 41.970 | 47.170  |
