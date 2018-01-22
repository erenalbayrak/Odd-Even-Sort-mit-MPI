#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

/**
 * This Example-Program read a binary file into a integer and print it out.
 * The content of the binary file should be also a integer array.
 *
 * Compile: g++ ReadBinaryFileIntoArray_Example.cpp -o ReadBinaryFileIntoArray_Example -std=gnu++0x
 * Example-Call with output: ./a.out "path/to/binayFile.bin" <y>
 */
int main(int argCount, char *argValues[])
{
    string binary_file = argValues[1];

    ifstream myFile(binary_file, ios::in | ios::binary);
    myFile.seekg(0, ios::end);

    long int count_all_bytes = myFile.tellg();
    int array_size           = count_all_bytes / sizeof(int);
    int *array               = new int[array_size];

    myFile.seekg(0, ios::beg);
    myFile.read(reinterpret_cast<char *>(array), sizeof(int) * array_size);

    if(argCount > 2 && strcmp(argValues[2], "y") == 0)
    {
        for (int j = 0; j < array_size; ++j) {
            cout << array[j] << " ";
        }
    }
    cout << endl;
}
