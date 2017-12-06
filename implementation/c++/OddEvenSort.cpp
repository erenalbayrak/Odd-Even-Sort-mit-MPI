#include <iostream>
#include <mpi.h>
#include <vector>

using namespace std;

/* the number of data elements in each process */
unsigned long long count_elements = 0;

/**
 * @param binary_file: the binary file on Disk.
 * @param vector: the values from binary file will store in the vector.
 * */
void fill_vector_from_binary_file(vector<int> *vector,
                                  char *binary_file,
                                  long rank,
                                  int count_nodes)
{
    FILE *filePtr = fopen(binary_file, "rb");
    fseek(filePtr, 0, SEEK_END);

    unsigned long count_all_elements = ftell(filePtr) / sizeof(int);
    rewind(filePtr);

    unsigned long vectorSize = count_all_elements / count_nodes;

    // Allocate memory for vector.
    vector->reserve(vectorSize + 5);
    auto *buffer = new int(1);

    for(long i=rank; i<count_all_elements; i=i+count_nodes)
    {
        fseek(filePtr, i, SEEK_SET);
        fread(buffer, 1, sizeof(int), filePtr);

        vector->push_back(*buffer);
    }
    delete(buffer);
}

/* print the data to the screen */
void print(vector<int> *vector, int rank)
{
    cout << "rank " << rank << " : ";
    for(unsigned long i=0; i<vector->size(); i++)
    {
        cout << vector->at(i) << " ";
    }
    cout << endl;
}

/* comparison function for qsort */
int cmp(const void* ap, const void* bp) {
  int a = * ((const int*) ap);
  int b = * ((const int*) bp);

  if (a < b) {
    return -1;
  } else if (a > b) {
    return 1;
  } else {
    return 0;
  }
}

/* find the index of the largest item in an array */
int max_index(int* data) {
  int i, max = data[0], maxi = 0;

  for (i = 1; i < count_elements; i++) {
    if (data[i] > max) {
      max = data[i];
      maxi = i;
    }
  }
  return maxi;
}

/* find the index of the smallest item in an array */
int min_index(int* data) {
  int i, min = data[0], mini = 0;

  for (i = 1; i < count_elements; i++) {
    if (data[i] < min) {
      min = data[i];
      mini = i;
    }
  }
  return mini;
}


/* do the parallel c++/even sort */
void parallel_sort(int* data, int rank, int size) {
  int i;

  /* the array we use for reading from partner */
  int other[count_elements];

  /* we need to apply P phases where P is the number of processes */
  for (i = 0; i < size; i++) 
  {
    /* sort our local array */
    qsort(data, count_elements, sizeof(int), &cmp);
    
    /* find our partner on this phase */
    int partener;

    /* if it's an even phase */
    if (i % 2 == 0) {
      /* if we are an even process */
      if (rank % 2 == 0) {
        partener = rank + 1;
      } else {
        partener = rank - 1;
      }
    } else {
      /* it's an c++ phase - do the opposite */
      if (rank % 2 == 0) {
        partener = rank - 1;
      } else {
        partener = rank + 1;
      }
    }

    /* if the partener is invalid, we should simply move on to the next iteration */
    if (partener < 0 || partener >= size) {
      continue;
    }

    /* do the exchange - even processes send first and c++ processes receive first
     * this avoids possible deadlock of two processes working together both sending */
    if (rank % 2 == 0) {
      MPI_Send(data, count_elements, MPI_INT, partener, 0, MPI_COMM_WORLD);
      MPI_Recv(other, count_elements, MPI_INT, partener, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else {
      MPI_Recv(other, count_elements, MPI_INT, partener, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Send(data, count_elements, MPI_INT, partener, 0, MPI_COMM_WORLD);
    }

    /* now we need to merge data and other based on if we want smaller or larger ones */
    if (rank < partener) {
      /* keep smaller keys */
      while (1) {
        /* find the smallest one in the other array */
        int mini = min_index(other);

        /* find the largest one in out array */
        int maxi = max_index(data);

        /* if the smallest one in the other array is less than the largest in ours, swap them */
        if (other[mini] < data[maxi]) {
          int temp = other[mini];
          other[mini] = data[maxi];
          data[maxi] = temp;
        } else {
          /* else stop because the smallest are now in data */
          break;
        }
      }
    } else {
      /* keep larger keys */
      while (1) {
        /* find the largest one in the other array */
        int maxi = max_index(other);

        /* find the largest one in out array */
        int mini = min_index(data);

        /* if the largest one in the other array is bigger than the smallest in ours, swap them */
        if (other[maxi] > data[mini]) {
          int temp = other[maxi];
          other[maxi] = data[mini];
          data[mini] = temp;
        } else {
          /* else stop because the largest are now in data */
          break;
        }
      }
    }
  }
}


/**
 * Compile: mpic++ OddEvenSort.cpp
 *
 * Example-Call: mpirun -n 4 ./a.out "numbers.bin"
 * */
int main(int argCount, char** argValues)
{
    int rank, count_nodes;

    MPI_Init(&argCount, &argValues);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &count_nodes);

    vector<int> vector;
    fill_vector_from_binary_file(&vector, argValues[1], rank, count_nodes);

    print(&vector, rank);

  /*
  parallel_sort(data, rank, size);
  
  printf("\nAfter parallel sorting:\n");
  print(data, rank);
  */

  MPI_Finalize();
  return 0;
}
