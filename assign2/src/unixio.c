#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <string.h>

#define RED     "\033[01;31m"
#define GREEN   "\033[32;1m"
#define RESET   "\033[0m"
#define YELLOW  "\033[33;3m"
#define BLUE    "\033[34;1m"
#define CYAN    "\033[01;36m"

void startTimer();
void stopTimer(char *func);
int isNumeric(char *str1);

struct timeval start,end;

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf(RED "usage: ./unixio filename numbytes calltype\nInvalid number of arguments." RESET);
        printf("\n");
        exit(EXIT_FAILURE);
    }

    char *filename = calloc(strlen(argv[1]) + 1,sizeof(char));
    strncpy(filename,argv[0],strlen(argv[1]));

    if(!isNumeric(argv[2]) ||!isNumeric(argv[3]))
    {
        printf(RED "usage: ./unixio filename numbytes calltype\nInvalid arguments." RESET);
        printf("\n");
        exit(EXIT_FAILURE);
    }

    int numBytes = (int) strtol(argv[2],NULL,0);  
    int type = (int) strtol(argv[3],NULL,0);

    if (type != 0 && type != 1)
    {
        printf(RED "usage: ./unixio filename numbytes calltype\nInvalid arguments." RESET);
        printf("\n");
        exit(EXIT_FAILURE);
    }

    if(type == 0)       //standard I/O C calls
    {
        printf("Using C functions to read a file by %d bytes per read\n", numBytes);

        FILE *file = fopen(filename,"r");
        if (file == NULL)
        {
            printf(RED "usage: ./unixio filename numbytes calltype\nFile specified not found." RESET);
            printf("\n");
            exit(EXIT_FAILURE);
        }

        startTimer();
        
        //use fgetc
        if (numBytes == 1)      
        {
            while(fgetc(file) != EOF)
            {
                //do nothing
            }
        }
        //use fread
        else
        {
            char *readStr = calloc(numBytes + 1, sizeof(char));

            while (fread(readStr, sizeof(char), numBytes, file) > 0)
            {
                //do nothing
            }
            free(readStr);
        }
        
        stopTimer("C fread's elapsed time = ");
        fclose(file);
    }
    else if(type == 1)      //unix system calls
    {
        printf("Using Unix I/O systems calls to read a file by %d bytes per read\n", numBytes);
        
        int filedescription = open(filename, O_RDONLY);
        if(filedescription < 0)
        {
            printf(RED "usage: ./unixio filename numbytes calltype\nFile specified not found." RESET);
            printf("\n");
            exit(EXIT_FAILURE);
        }

        startTimer();
        char *readStr = calloc(numBytes + 1, sizeof(char));

        while (read(filedescription, readStr, numBytes) > 0)  
        {
            //do nothing
        }
        
        free(readStr);
        stopTimer("Unix read's elapsed time = ");
        close(filedescription);
    }
    return 0;
}

void startTimer()
{
    gettimeofday(&start,NULL);
}

void stopTimer(char *func)
{
    gettimeofday(&end,NULL);
    printf("%s%ld\n", func, (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec));
}

int isNumeric(char *str1)
{
    int i = 0;
    int len = strlen(str1);

    for (i = 0; i < len; i++)
    {
        if (!isdigit(str1[i]))
            return 0;
    }
    return 1;
}
