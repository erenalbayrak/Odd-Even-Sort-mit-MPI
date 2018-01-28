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

| Count Processes | 15     | 30     | 60     | 120    | 240     | 480     |
|-----------------|--------|--------|--------|--------|---------|---------|
| Count Values    |        |        |        |        |         |         |
| 480             | 0,985  | 1,367  | 2,022  | 3,521  | 6,517   | 13,477  |
| 4.800           | 0,976  | 1,355  | 1,938  | 3,444  | 6,545   | 12,657  |
| 48.000          | 1,027  | 1,368  | 2,064  | 3,539  | 6,775   | 12,711  |
| 480.000         | 1,630  | 2,001  | 2,662  | 4,071  | 7,441   | 13,651  |
| 4.800.000       | 7,566  | 7,840  | 8,545  | 11,537 | 15,479  | 22,729  |
| 48.000.000      | 71,458 | 71,589 | 71,076 | 75,547 | 103,359 | 105,701 |

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

| Count Processes: | 4     | 8       | 16     | 32     | 64    | 128    | 256     |np/qs    |timsort  |odd/even |
|------------------|-------|---------|--------|--------|-------|--------|---------|---------|---------|---------|
| Count Values     |       |         |        |        |       |        |         |         |         |         |
| 128	   	       | 1.963 | 1.853	 | 2.042  | 2.322  | 3.110 | 4.663  | N/A 	 | 0.810   | 0.749   | 0.793   |
| 4096	          | 1.855 | 1.921	 | 2.022  | 2.370  | 3.093 | 4.732  | 8.909   | 0.770   | 0.813   | 41.529  |
| 131072	          | 2.066 | 2.025	 | 2.344  | 2.698  | 3.633 | 4.840  | 8.478   | 0.769   | 1.55    | N/A     |
| 4194304	       | 5.353 | 5.722	 | 6.225  | 6.654  | 7.312 | 9.851  | 13.817  | 1.680   | 36.631  | N/A   	 |
| 16777216         | 18.66 | 18.513	 | 19.546 | 22.012 | 22.13 | 24.340 | 29.714  | 4.933   | N/A     | N/A     |
| 33554432         | N/A   | N/A	    | 52.181 | 41.715 | 39.72 | 41.970 | 47.170  | 9.292   | N/A     | N/A     |

np/qs = quicksort with python library numpy on 1 core
timsort = default python "sorted(...)" on 1 core
odd/even = simple odd even sort on 1 core

