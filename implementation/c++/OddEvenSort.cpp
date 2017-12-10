#include <iostream>
#include <mpi.h>
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;

/**
 * @param binary_file: the binary file on Disk.
 * @param vector: the values from binary file will store in the vector.
 * */
void fill_vector_from_binary_file(vector<int> *vector,
                                  char *binary_file,
                                  long rank,
                                  int count_nodes)
{
    ifstream myFile(binary_file, ios::in | ios::binary);
    myFile.seekg(0, ios::end);

    long int count_all_bytes = myFile.tellg();
    long int vector_size              = (count_all_bytes / sizeof(int) / count_nodes);

    // Allocate memory for vector.
    vector->reserve((unsigned long) vector_size);

    int a;
    for(long int i = rank * sizeof(int);
        i < count_all_bytes;
        i = i + sizeof(int) * count_nodes)
    {
        myFile.seekg(i, ios::beg);
        myFile.read(reinterpret_cast<char *>(&a), sizeof(a));
        vector->push_back(a);
    }
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

    for( unsigned long int i=1; i<data->size(); i++ )
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

        /* The data from our partner. */
        auto *other = new vector<int>;
        MPI_Status status;
        int count_other;

        /* do the exchange - even processes send first and odd processes receive first
         * this avoids possible deadlock of two processes working together both sending */
        if (rank % 2 == 0) {
            MPI_Send(&data->at(0), (int) data->size(), MPI_INT, partener, 0, MPI_COMM_WORLD);

            MPI_Probe(partener, 0, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &count_other);
            other->resize((unsigned long) count_other);

            MPI_Recv( &other->at(0), count_other, MPI_INT, partener, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else
        {
            MPI_Probe(partener, 0, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &count_other);
            other->resize((unsigned long) count_other);

            MPI_Recv(&other->at(0), count_other, MPI_INT, partener, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(data, (int) data->size(), MPI_INT, partener, 0, MPI_COMM_WORLD);
        }

        /* now we need to merge data and other based on if we want smaller or larger ones */
        if (rank < partener) {
          /* keep smaller keys */
          while (true)
          {
              /* find the smallest one in the other array */
              unsigned long int mini = min_index(other);

              /* find the largest one in out array */
              unsigned long int maxi = max_index(data);

                /* if the smallest one in the other array is less than the largest in ours, swap them */
                if (other->at(mini) < data->at(maxi))
                {
                    int temp = other->at(mini);
                    other->at(mini) = data->at(maxi);
                    data->at(maxi) = temp;
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
              unsigned long int maxi = max_index(other);

              /* find the largest one in out array */
              unsigned long int mini = min_index(data);

              /* if the largest one in the other array is bigger than the smallest in ours, swap them */
              if (other->at(maxi) > data->at(mini))
              {
                  int temp = other->at(maxi);
                  other->at(maxi) = data->at(mini);
                  data->at(mini) = temp;
              } else {
                /* else stop because the largest are now in data */
                break;
            }
          }
        }
        delete(other);
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
