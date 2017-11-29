#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/**
 * On success, the first element of argValues (argValues[0]) will overwritten with the string, which representing the count of numbers. argValues[0] will needed later for beeing part of the resulting filename.
 * @returns on success: the count of numbers.
 *          on failure: EXIT_FAILURE;
 */
unsigned long long getCountNumbersToGenerate(int argCount, char *argValues[]) 
{
    /*
     * Iterating throug the arguments to search the value of -countNumbersToGenerate.
     * Start with i=1, because the first argument is the name of this program.
     * */
    int i;
    for(i=1; i<argCount; i++) 
    {
        if( strcmp(argValues[i], "-countNumbersToGenerate") == 0
                &&
            ++i < argCount)
        {   
            argValues[0] = argValues[i];
            return atoll(argValues[i]);
        }
    }
    
    return EXIT_FAILURE;
}


char* getCreateFileName(char *strCountNumbersToGenerate)
{
    time_t rawtime;
    time (&rawtime);
    
    char strActualTime[30];
    strcpy(strActualTime, ctime(&rawtime));
    strActualTime[strlen(strActualTime)-1] = '\0';
    
    int fileNameLength = strlen(strCountNumbersToGenerate) + 
                         strlen(" numbers - ") + 
                         strlen(strActualTime) + 
                         strlen(".txt") + 1;
    char *fileName;
    fileName = (char*) malloc(fileNameLength * sizeof(char));
    
    strcpy(fileName, strCountNumbersToGenerate);
    strcat(fileName, " numbers - ");
    strcat(fileName, strActualTime);
    strcat(fileName, ".txt");
    
    return fileName;
}


/** 
 * This program generates random numbers.
 * The result will save into a file named "<countNumbersToGenerate> numbers -<Current Date>.txt
 * Example: "1000 numbers - Tue Oct 31 15:41:15 2017.txt"
 * The file will created in the same directory, where this program will executed."
 * 
 * @param (mandatory): -countNumbersToGenerate X where X is the count of desired random numbers. Data type is unsigned long long
 * 
 * Example call: ./RandomNumbers -countNumbersToGenerate 1600
 */
int main(int argCount, char *argValues[]) 
{
    unsigned long long countNumbersToGenerate = getCountNumbersToGenerate(argCount, argValues);
    
    if(countNumbersToGenerate == EXIT_FAILURE) 
    {
        printf("The Argument -countNumbersToGenerate is needed.\n");
        printf("Example call: ./RandomNumberGenerator -countNumbersToGenerate 1600\n");
        
        return EXIT_FAILURE;
    }
    
    
    char *fileName   = getCreateFileName(argValues[0]);
    FILE *fileResult = fopen(fileName, "w");
    if(fileResult == NULL) 
    {
        printf("Error. Can't open the result file.\n");
        return EXIT_FAILURE;
    }
    
    srand(time(NULL));
    unsigned long long i;
    for(i=0; i<countNumbersToGenerate; i++) 
    {
        if(i+1 < countNumbersToGenerate)
            fprintf( fileResult, "%d,", rand() );
        else
            fprintf( fileResult, "%d", rand() );
    }
    
    printf("Resultfile was created: \"%s\"\n", fileName);
    
    fclose(fileResult);
    free(fileName);
    
    return EXIT_SUCCESS;
}
