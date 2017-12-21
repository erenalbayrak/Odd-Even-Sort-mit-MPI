#include <iostream>
#include <ctime>
#include <string>
#include <cstring>

using namespace std;

string create_file_name(char *strCountNumbersToGenerate)
{
    time_t rawtime;
    time (&rawtime);

    string str_time  = ctime(&rawtime);
    string file_name = strCountNumbersToGenerate;
    file_name += " numbers - ";
    file_name += str_time.substr(0,str_time.length()-1);
    file_name += ".bin";

    return file_name;
}


/** 
 * This program generates random numbers.
 * The result will save into a file named "<countNumbersToGenerate> numbers - <Current Date>.bin"
 * Example: "1000 numbers - Tue Oct 31 15:41:15 2017.bin"
 * The file will created in the same directory, where this program will executed."
 *
 * Compile: g++ RandomNumberGenerator.cpp -o RandomNumberGenerator -std=gnu++0x
 * Call with output on console:     ./RandomNumbers 1600 y
 * Call without output on console:  ./RandomNumbers 1600
 */
int main(int argCount, char *argValues[]) 
{
    string file_name = create_file_name(argValues[1]);
    FILE *resultFile = fopen(file_name.c_str(), "wb");
    if(resultFile == nullptr)
    {
        cout << "Error. Can't open the result file." << endl;
        return EXIT_FAILURE;
    }

    srand(time(NULL));
    long long int numbers_to_generate = atoll(argValues[1]);
    for(int i=0; i<numbers_to_generate; i++)
    {   
        int randNumber = rand();
        fwrite(&randNumber, sizeof randNumber, 1, resultFile);

        if( argCount > 2 && strcmp(argValues[2], "y")==0 )
        {
            cout << randNumber << " ";
        }
    }
    
    cout << endl << "Result file created: " <<  file_name << endl;
    
    fclose(resultFile);
    return EXIT_SUCCESS;
}
