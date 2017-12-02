#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * This Example-Program read a binary file into a integer array (and print it out if argument "y" is set).
 * The content of the binary file should be also a integer array.
 * 
 * Example-Call with output: ./a.out "path/to/binayFile.bin" y
 * Example-Call no output:   ./a.out "path/to/binayFile.bin"
 */
int main(int argCount, char *argValues[])
{
    FILE *filePtr = fopen(argValues[1], "rb");      // Open the file in binary mode
    fseek(filePtr, 0, SEEK_END);                    // Jump to the end of the file
    
    unsigned long long fileLength = ftell(filePtr); // Get the current byte offset in the file
    fileLength = fileLength / sizeof(int);
    rewind(filePtr);                                // Jump back to the beginning of the file
    
    int *buffer = malloc(sizeof(int) * (fileLength));
    fread(buffer, fileLength, sizeof(int), filePtr); // Read the entire file
    
    printf("File lenght / sizeof(int) = %d Bytes\n", fileLength);
    
    if( argCount == 3 && strcmp(argValues[2], "y") == 0 )
    {
        unsigned long long i=0;
        while(i < fileLength)
        {
            printf("%d. = %d\n", i, buffer[i]);
            i++;
        }
    }
    
    fclose(filePtr);
}
