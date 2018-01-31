#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

using namespace std;

/**
 * This Example-Program read a binary file into a vector<int> and can print it out.
 * The content of the binary file should be also a integer array.
 *
 * Compile: g++ ReadBinaryFileIntoVector_Example.cpp -o ReadBinaryFileIntoVector_Example -std=gnu++0x
 * Example-Call: ./ReadBinaryFileIntoVector_Example "path/to/binayFile.bin" <y>
 * y to output on console.
 */
int main(int argCount, char *argValues[])
{
    string binary_file = argValues[1];

    ifstream myFile(binary_file, ios::in | ios::binary);
    myFile.seekg(0, ios::end);

    long int count_all_bytes = myFile.tellg();
    int vec_size             = count_all_bytes / sizeof(int);

    vector<int> vector(vec_size);

    myFile.seekg(0, ios::beg);
    myFile.read(reinterpret_cast<char *>(&vector.at(0)), sizeof(int) * vec_size);

    if(argCount > 2 && strcmp(argValues[2], "y") == 0)
    {
        for (int j = 0; j < vector.size(); ++j) {
            cout << vector.at(j) << " ";
        }
    }
    cout << endl;
}