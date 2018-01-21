#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

using namespace std;

/**
 * This Example-Program read a binary file into a integer and print it out.
 * The content of the binary file should be also a integer array.
 * 
 * Example-Call with output: ./a.out "path/to/binayFile.bin" <y>
 */
int main(int argCount, char *argValues[])
{
    string binary_file = argValues[1];

    ifstream myFile(binary_file, ios::in | ios::binary);
    myFile.seekg(0, ios::end);

    long int count_all_bytes = myFile.tellg();
    int vec_size             = count_all_bytes / sizeof(int);
    vector<int> vector(vec_size);
    int a, iVec = 0;

    for(long int i=0; i<count_all_bytes; i = i + sizeof(int))
    {
        myFile.seekg(i, ios::beg);
        myFile.read(reinterpret_cast<char *>(&a), sizeof(a));
        /*
         * push_back do always a bad alloc exception if dealing with size > 1 GB
         * vector.push_back(a);
         * */
        vector.at(iVec++) = a;
    }

    if(argCount > 2 && strcmp(argValues[2], "y") == 0) {
        for (int j = 0; j < vector.size(); ++j) {
            cout << vector.at(j) << " ";
        }
    }
    cout << endl;
}
