#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

int NITER = 1000000;

int cnt = 0;

void * Count(void * a)
{
    int i, tmp;
    for(i = 0; i < NITER; i++)
    {
        tmp = cnt;      /* copy the global cnt locally */
        tmp = tmp+1;    /* increment the local copy */
        cnt = tmp;      /* store the local value into the global cnt */ 
    }
}

int main(int argc, char * argv[])
{
	pthread_t tid1, tid2;

	// validate arguments
	// implementation


	// Parsing the arguments passed to your C program
	// Including the number of times that each thread increments
	// the shared count cnt
	// For example, NITER = 20000;


	// Display the number of times (entered by the user) that each thread
	// increments the shared count cnt
// PLEASE DO NOT remove or modify the following code 
	printf("2*NITER is [%d]\n", 2*NITER);
// End of code section 

	// creating Thread 1
	if(pthread_create(&tid1, NULL, Count, NULL))
	{
		printf("\n ERROR creating thread 1");
		exit(1);
	}

	// creating Thread 2
	if(pthread_create(&tid2, NULL, Count, NULL))
	{
		printf("\n ERROR creating thread 2");
		exit(1);
	}

	if(pthread_join(tid1, NULL))	/* wait for the thread 1 to finish */
	{
		printf("\n ERROR joining thread");
		exit(1);
	}

	if(pthread_join(tid2, NULL))        /* wait for the thread 2 to finish */
	{
		printf("\n ERROR joining thread");
		exit(1);
	}

        // Display the value of count cnt
// PLEASE DO NOT remove or modify the following code
	printf("\nCounter is [%d]\n", cnt);
	if (cnt == 2 * NITER) 
		printf("\nPASS\n");
	else
		printf("\nFAIL\n");
// End of code section

	pthread_exit(NULL);
}


