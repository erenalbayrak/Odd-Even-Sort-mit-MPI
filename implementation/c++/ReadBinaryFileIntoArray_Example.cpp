#include <iostream>
#include <fstream>

using namespace std;

/**
 * This Example-Program read a binary file into a integer and print it out.
 * The content of the binary file should be also a integer array.
 * 
 * Example-Call with output: ./a.out "path/to/binayFile.bin"
 */
int main(int argCount, char *argValues[])
{
    string binary_file = argValues[1];

    ifstream myFile(binary_file, ios::in | ios::binary);
    myFile.seekg(0, ios::end);

    long int count_all_bytes = myFile.tellg();
    int a;

    for(long int i=0; i<count_all_bytes; i = i + sizeof(int))
    {
        myFile.seekg(i, ios::beg);
        myFile.read(reinterpret_cast<char *>(&a), sizeof(a));
        cout << a << " ";
    }
    cout << endl;
}
