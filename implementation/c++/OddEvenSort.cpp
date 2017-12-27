#include <iostream>
#include <mpi.h>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cstring>

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
    ifstream bin_file(binary_file, ios::in | ios::binary);
    bin_file.seekg(0, ios::end);

    long int count_all_bytes = bin_file.tellg();
    long int vector_size     = (count_all_bytes / sizeof(int) / count_nodes);

    // Allocate memory for vector.
    vector->reserve((unsigned long) vector_size);

    int actual_value;
    for(long int i = rank * sizeof(int);
        i < count_all_bytes;
        i = i + sizeof(int) * count_nodes)
    {
        bin_file.seekg(i, ios::beg);
        bin_file.read(reinterpret_cast<char *>(&actual_value), sizeof(actual_value));
        vector->push_back(actual_value);
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

/* do the parallel odd/even sort */
void parallel_sort(vector<int> *data, int rank, int size)
{
    /* The data from our partner. */
    vector<int> *other;
    vector<int>::iterator dataIterator;
    MPI_Status status;
    int count_other;

    /* we need to apply P phases where P is the number of processes */
    for (int i=0; i<size; i++)
    {
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

        // get empty space for next round getting data from partner.
        other = new vector<int>;

        /* do the exchange - even processes send first and odd processes receive first
         * this avoids possible deadlock of two processes working together both sending */
        if (rank % 2 == 0)
        {
            MPI_Send(&data->at(0), (int) data->size(), MPI_INT, partener, 0, MPI_COMM_WORLD);

            // first get the count of elements from from partner, than alloc exat the needed space.
            MPI_Probe(partener, 0, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &count_other);
            other->resize((unsigned long) count_other);

            MPI_Recv( &other->at(0), count_other, MPI_INT, partener, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        else
        {
            // first get the count of elements from from partner, than alloc exat the needed space.
            MPI_Probe(partener, 0, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &count_other);
            other->resize((unsigned long) count_other);

            MPI_Recv(&other->at(0), count_other, MPI_INT, partener, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(&data->at(0), (int) data->size(), MPI_INT, partener, 0, MPI_COMM_WORLD);
        }

        /* now we need to merge data and other based on if we want smaller or larger ones */
        if (rank < partener)
        {
            /* keep smaller keys */
            for(unsigned long iOther = 0; iOther < other->size(); iOther++)
            {
                int actOtherValue = other->at(iOther);
                if (actOtherValue < data->back())
                {
                    dataIterator = lower_bound(data->begin(), data->end(), actOtherValue);
                    //cout << "AAA" << endl;
                    data->insert(dataIterator, actOtherValue);
                    //cout << "BBB" << endl;
                    data->erase(data->end()-1);
                    //cout << "CCC" << endl;
                }
                else
                    break;
            }
        }
        else
        {
            /* keep larger keys */
            for(unsigned long iOther = other->size()-1; iOther >= 0; iOther--)
            {
                int actOtherValue = other->at(iOther);
                if (actOtherValue > data->front())
                {
                    //cout << "act val = " << actOtherValue << endl;
                    dataIterator = upper_bound(data->begin(), data->end(), actOtherValue);
                    //cout << "AAA" << endl;
                    data->insert(dataIterator, actOtherValue);
                    //cout << "BBB" << endl;
                    data->erase(data->begin());
                    //cout << "CCC" << endl;
                }
                else
                    break;
            }
        }
        delete(other);
    }
}


/**
 * Compile: mpic++ OddEvenSort.cpp
 *
 * Example-Call: mpirun -n 4 ./a.out "<numbers_file.bin>" <y or nothing>
 * y == output on console
 * */
int main(int argCount, char** argValues)
{
    int rank, count_nodes;

    MPI_Init(&argCount, &argValues);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &count_nodes);

    vector<int> vec_data;
    fill_vector_from_binary_file(&vec_data, argValues[1], rank, count_nodes);

    /* sort our local vector */
    sort(vec_data.begin(), vec_data.end());

    parallel_sort(&vec_data, rank, count_nodes);

    if(argCount > 2 && strcmp(argValues[2], "y") == 0)
    {
        print(&vec_data, rank);
    }

    MPI_Finalize();
    return 0;
}
