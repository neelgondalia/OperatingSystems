#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int NITER = 1000000;
int cnt = 0;
sem_t mutex;

void * Count(void * a)
{
    int i, tmp;
    for(i = 0; i < NITER; i++)
    {
		sem_wait(&mutex); /*waits on a semaphore*/
        tmp = cnt;      /* copy the global cnt locally */
        tmp = tmp+1;    /* increment the local copy */
        cnt = tmp;      /* store the local value into the global cnt */ 
		sem_post(&mutex); /*increments the value of semaphore */
	}
}

int main(int argc, char * argv[])
{
	pthread_t tid1, tid2;
	
	// validate arguments
	//checks the number of arguments
	if (argc != 2)
	{
		fprintf(stderr, "usage: %s NoofTimesEachThreadIncrements\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	//checks if inputted argument is numeric
	int i = 0;
	for (i = 0; i < strlen(argv[1]); i++)
	{
		if (!isdigit(argv[1][i]))
		{
			fprintf(stderr, "usage: %s NoofTimesEachThreadIncrements\n", argv[0]);
			exit(EXIT_FAILURE);	
		}
	}

	//inputted number of times each thread increments the shared counter
	NITER = atoi(argv[1]);

	//initialize the semaphore
	sem_init(&mutex,0,1);

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

	//destroy the semaphore
	sem_destroy(&mutex);
	pthread_exit(NULL);
}


