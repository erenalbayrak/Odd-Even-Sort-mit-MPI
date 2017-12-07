#include <iostream>
#include <mpi.h>
#include <vector>
#include <algorithm>

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
    unsigned long vectorSize         = (count_all_elements / count_nodes) + 5;

    vector->reserve(vectorSize); // Allocate memory for vector.
    auto *buffer = new int(1);
    rewind(filePtr);

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
    for(unsigned long int i=0; i<vector->size(); i++)
    {
        cout << vector->at(i) << " ";
    }
    cout << endl;
}

/* find the index of the largest item in an array */
unsigned long int max_index(vector<int> *data)
{
    int max = data->at(0);
    unsigned long int maxi = 0;

    for( unsigned long int i = 1; i < data->size(); i++ )
    {
        if(data->at(i) > max)
        {
          max = data->at(i);
          maxi = i;
        }
    }
    return maxi;
}

/* find the index of the smallest item in an array */
unsigned long int min_index(vector<int> *data)
{
    int min = data->at(0);
    unsigned long int mini = 0;

    for( unsigned long int i=1; i<count_elements; i++ )
    {
        if(data->at(i) < min)
        {
          min = data->at(i);
          mini = i;
        }
    }
  return mini;
}


/* do the parallel odd/even sort */
void parallel_sort(vector<int> *data, int rank, int size)
{
    /* the array we use for reading from partner */
    vector <int> other;
    other.resize(data->size());

    /* we need to apply P phases where P is the number of processes */
    for (int i=0; i<size; i++)
    {
        /* sort our local array */
        sort(data->begin(), data->end());

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
          /* it's an odd phase - do the opposite */
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

        /* do the exchange - even processes send first and odd processes receive first
         * this avoids possible deadlock of two processes working together both sending */
        if (rank % 2 == 0) {
            print(data, rank);
          MPI_Send(data, (int) data->size(), MPI_INT, partener, 0, MPI_COMM_WORLD);
          MPI_Recv(&other, (int) other.size(), MPI_INT, partener, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            print(&other, rank);
        } else {
          MPI_Recv(&other, (int) other.size(), MPI_INT, partener, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
          MPI_Send(data, (int) data->size(), MPI_INT, partener, 0, MPI_COMM_WORLD);
        }

        /* now we need to merge data and other based on if we want smaller or larger ones */
        if (rank < partener) {
          /* keep smaller keys */
          while (true)
          {
              /* find the smallest one in the other array */
              unsigned long int mini = min_index(&other);

              /* find the largest one in out array */
              unsigned long int maxi = max_index(data);

                /* if the smallest one in the other array is less than the largest in ours, swap them */
                if (other.at(mini) < data->at(maxi))
                {
                    int temp = other.at(mini);
                    other.insert( other.begin() + mini, data->at(maxi) );
                    data->insert( data->begin() + maxi, temp );
                } else {
                  /* else stop because the smallest are now in data */
                  break;
                }
          }
        }
        else
        {
          /* keep larger keys */
          while (true)
          {
              /* find the largest one in the other array */
              unsigned long int maxi = max_index(&other);

              /* find the largest one in out array */
              unsigned long int mini = min_index(data);

              /* if the largest one in the other array is bigger than the smallest in ours, swap them */
              if (other.at(maxi) > data->at(mini))
              {
                  int temp = other.at(maxi);
                  other.insert( other.begin() + maxi, data->at(mini) );
                  data->insert( data->begin() + mini, temp );
              } else {
                /* else stop because the largest are now in data */
                break;
            }
          }
        }
        other.resize(data->size());
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

    vector<int> vecData;
    fill_vector_from_binary_file(&vecData, argValues[1], rank, count_nodes);

    //print(&vecData, rank);
    parallel_sort(&vecData, rank, count_nodes);

    cout << endl << "After parallel sorting:" << endl;
    print(&vecData, rank);

    MPI_Finalize();
    return 0;
}
