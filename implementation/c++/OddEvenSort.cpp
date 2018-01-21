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
 * @return EXIT_SUCCESS or EXIT_FAILURE
 * */
int fill_vector_from_binary_file(vector<int> *vector,
                                  char *binary_file,
                                  long rank,
                                  int count_processes)
{
    ifstream bin_file(binary_file, ios::in | ios::binary);
    bin_file.seekg(0, ios::end);

    const long int count_all_bytes  = bin_file.tellg();
    const unsigned long vector_size = (count_all_bytes / sizeof(int) / count_processes);

    if(vector_size < 1) {
        cout << "The amount of processes is higher than the amount of values. In this case not every process will become values." << endl;
        return EXIT_FAILURE;
    }
    else if( (count_all_bytes / sizeof(int)) % count_processes != 0  ) {
        cout << "The amount of values have to be even on every process. Otherwise the sorting would be incorrect." << endl;
        return EXIT_FAILURE;
    }

    // Allocate memory for vector.
    vector->resize(vector_size);

    int actual_value;
    unsigned long iVector = 0;
    for(long int i = rank * sizeof(int);
        i < count_all_bytes;
        i = i + sizeof(int) * count_processes)
    {
        bin_file.seekg(i, ios::beg);
        bin_file.read(reinterpret_cast<char *>(&actual_value), sizeof(actual_value));
        /*
         * push_back do always a bad alloc exception if dealing with size > 1 GB
         * vector->push_back(actual_value);
         * */
        vector->at(iVector++) = actual_value;
    }
    return EXIT_SUCCESS;
}

void print(vector<int> *vector, int rank)
{
    cout << "rank " << rank << " : ";
    for(unsigned long int i=0; i<vector->size(); i++)
        cout << vector->at(i) << " ";
    cout << endl;
}

int findPartner(int phase, int rank) {
    int partner;

    /* if it's an even phase */
    if (phase % 2 == 0) {
        /* if we are an even process */
        if (rank % 2 == 0) {
            partner = rank + 1;
        } else {
            partner = rank - 1;
        }
    } else {
        /* it's an odd phase - do the opposite */
        if (rank % 2 == 0) {
            partner = rank - 1;
        } else {
            partner = rank + 1;
        }
    }
    return partner;
}

void parallel_sort(vector<int> *data, int rank, int count_processes)
{
    const unsigned long vector_size = data->size();

    vector<int> other(vector_size);
    vector<int> concatVec(vector_size * 2);

    for (int i=0; i<count_processes; i++)
    {
        int partner = findPartner(i, rank);
        if (partner < 0 || partner >= count_processes)
          continue;

        if (rank % 2 == 0) {
            MPI_Send(&data->at(0), (int) vector_size, MPI_INT, partner, 0, MPI_COMM_WORLD);
            MPI_Recv(&other.at(0), (int) vector_size, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else {
            MPI_Recv(&other.at(0), (int) vector_size, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(&data->at(0), (int) vector_size, MPI_INT, partner, 0, MPI_COMM_WORLD);
        }

        merge( data->begin(), data->end(), other.begin(), other.end(), concatVec.begin() );
        sort(concatVec.begin(), concatVec.end());

        auto posHalfVec = concatVec.begin() + vector_size;
        if (rank < partner)
            data->assign(concatVec.begin(), posHalfVec);
        else
            data->assign(posHalfVec, concatVec.end());
    }
}

/**
 * Compile: mpic++ OddEvenSort.cpp -o OddEvenSort -std=gnu++0x
 *
 * Example-Call: mpirun -np 4 ./a.out "<numbers_file.bin>" <y>
 * <y> output on console
 * time mpirun -np 2 ./OddEvenSort "16 numbers - Sat Jan 20 14:02:43 2018.bin" y
 * */
int main(int argCount, char** argValues)
{
    int rank, count_processes;

    MPI_Init(&argCount, &argValues);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &count_processes);

    vector<int> vec_data;
    int status = fill_vector_from_binary_file(&vec_data, argValues[1], rank, count_processes);

    if (status == EXIT_FAILURE) {
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    parallel_sort(&vec_data, rank, count_processes);

    if(argCount > 2 && strcmp(argValues[2], "y") == 0) {
        print(&vec_data, rank);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
