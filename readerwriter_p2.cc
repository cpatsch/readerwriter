/**************************************************************************

Reader/Writer Problem - Threads

Programmer: 	Caleb Patsch
Date:			11/5/2023

Purpose:	This program creates reader and writer threads. Writers
		chop off the last letter of a string, and readers print
		them. In this simulation, readers and writers alternate.

**************************************************************************/
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

sem_t write_sem;
sem_t read_sem;
int write_count;
int read_count;

char str[] = "All work and no play makes Jack a dull boy.";

/**************************************************************************

Function:	usage()

Use:		Prints the usage of the program.

Arguments:	None.

Returns:	Nothing.

**************************************************************************/

void usage()
{
	fprintf(stderr,"\n");
	fprintf(stderr,"Usage: ./readerwriter_p2 [num_readers] [num_writers]\n");
	fprintf(stderr,"======================================================\n");
	fprintf(stderr,"[num_readers] - number of reading threads.\n");
	fprintf(stderr,"[num_writers] - number of writing threads.\n");
	fprintf(stderr,"\n");
}

/**************************************************************************

Function:	reader()

Use:		The reader thread. It prints the value of
		the shared string.

Arguments:	1. *param: The id sent to the reader thread by
		           pthread_create().

Returns:	Nothing.

**************************************************************************/

void *reader(void *param)
{
	// Get the thread's id.
	long id = (long) param;

	// Loop while the string is not empty.
	while(strlen(str) != 0)
	{
		// Wait for read semaphore. If it fails, print why.
		if(sem_wait(&read_sem) != 0)
		{
			fprintf(stderr,"sem_wait(): reader semaphore error - %s.\n",strerror(errno));
			exit(-1);
		}


		// Print value of string.
		printf("reader %ld is reading ... content : %s\n",id,str);


		// Signal the write semaphore.
		if(sem_post(&write_sem) != 0)
		{
			// Print error.
			fprintf(stderr,"sem_post(): writer semaphore error - %s.\n",strerror(errno));
			exit(-1);
		}

		// Sleep for 1 second.
		if(sleep(1) != 0)
		{
			// Print error on fail.
			fprintf(stderr,"sleep(): %s.\n",strerror(errno));
			exit(-1);
		}
	}

		// Notify to user that the reader is exiting.
		printf("reader %ld is exiting ...\n",id);

		// Get remaining writers.
		int remaining_writers = write_count - read_count;

		// Sleep for 1 second. This makes it so that the writer can exit before
		// it signals the rest of the writers.
		if(sleep(1) != 0)
		{
			// Print error on fail.
			fprintf(stderr,"sleep(): %s.\n",strerror(errno));
			exit(-1);
		}

		// Print how many writers are remaining.
		printf("There are still %d writers waiting on the semaphore...\n",remaining_writers);

		// Signal all writers to stop.
		for (int i = 0; i < remaining_writers; i++)
		{
			// Signal the writers. If it fails, print why.
			if(sem_post(&write_sem) != 0)
			{
				fprintf(stderr,"sem_post(): writer semaphore error - %s.\n",strerror(errno));
				exit(-1);
			}
		}

		// Exit thread.
		pthread_exit(0);
}

/**************************************************************************

Function:	writer()

Use:		The writer thread. It chops of the last letter of the
		string.

Arguments:	1. *param: The id sent to the writer thread by
		           pthread_create().

Returns:	Nothing.

**************************************************************************/

void *writer(void *param)
{
	// Get the thread's id.
	long id = (long) param;

	// Loop while the string isn't empty.
	while (strlen(str) != 0)
	{
		// Wait for the write semaphore.
		if(sem_wait(&write_sem) != 0)
		{
			// Print error on fail.
			fprintf(stderr,"sem_wait(): write semaphore error - %s.\n",strerror(errno));
			exit(-1);
		}

		// Check again if the string is empty. This is so that
		// there isn't needless writing if it is.
		if (strlen(str) != 0)
		{
			// Print that the writer is writing.
			printf("writer %ld is writing ...\n",id);
			// Write by replacing the last character to a null
			// terminating.
			str[strlen(str)-1] = '\0';
		}

		// Signal the reader to continue.
		if(sem_post(&read_sem) != 0)
		{
			// Print an error on fail.
			fprintf(stderr,"sem_post(): read semaphore error - %s.\n",strerror(errno));
			exit(-1);
		}

		// Sleep for 1 second.
		if (sleep(1) != 0)
		{
			// Print error on fail.
			fprintf(stderr,"sleep(): %s.\n",strerror(errno));
			exit(-1);
		}
	}

	// Notify to user that the writer is exiting.
	printf("writer %ld is exiting ...\n",id);
	// Exit thread.
	pthread_exit(0);
}

/**************************************************************************

Function:	check_args()

Use:		Checks the arguments passed to the program, and
		verifies that they are valid numbers.

Arguments:	1. argc: The number of arguments.
		2. *argv[]: A char * string that holds the arguments.

Returns:	Nothing.

**************************************************************************/

void check_args(int argc, char *argv[])
{
	// Check if there are not 3 arguments.
	if (argc != 3)
	{
		// Print the usage then exit.
		usage();
		exit(-1);
	}

	// Check if the first argument is under 0.
	if (atoi(argv[1]) < 0)
	{
		// Print error.
		fprintf(stderr,"number of readers must be greater than 0.\n");
		exit(-1);
	}
	// Else, check if it equals 0.
	else if (atoi(argv[1]) == 0)
	{
		// Print error.
		fprintf(stderr,"number of readers must be a valid number.\n");
		exit(-1);
	}

	// Check if the second is under 0.
	if (atoi(argv[2]) < 0)
	{
		// Print error.
		fprintf(stderr,"number of writers must be greater than 0.\n");
		exit(-1);
	}
	// Else, check if it equals 0.
	else if (atoi(argv[2]) == 0)
	{
		// Print error.
		fprintf(stderr,"number of writers must be a valid number.\n");
		exit(-1);
	}
}

/**************************************************************************

Function:	init_vars()

Use:		Initializes the global variables.

Arguments:	None.

Returns:	Nothing.

**************************************************************************/

void init_vars()
{
	// Try to initialize the write semaphore.
	if(sem_init(&write_sem, 0, 0) != 0)
	{
		// If it fails, print an error.
		fprintf(stderr,"sem_init(): write semaphore error - %s.\n",strerror(errno));
		exit(-1);
	}
	// Try to initialize the read semaphore.
	if(sem_init(&read_sem, 0, 1) != 0)
	{
		// If it fails, print an error.
		fprintf(stderr,"sem_init(): read semaphore error - %s.\n",strerror(errno));
		exit(-1);
	}

	// Set write_count to 0.
	write_count = 0;

	// Set read_count to 0.
	read_count = 0;
}

/**************************************************************************

Function:	create_rws()

Use:		Creates the reader and writer threads.

Arguments:	1. *argv[]: char * string that holds the arguments.

Returns:	Nothing.

**************************************************************************/

void create_rws(char *argv[])
{
	// Print the header.
	printf("*** Reader-Writer Problem Simulation ***\n");
	printf("Number of reader threads: %d\n",atoi(argv[1]));
	printf("Number of writer threads: %d\n",atoi(argv[2]));

	// Initialize reader and writer arrays, set to the amount of reader and
	// writers, respectively.
	pthread_t rtid[atoi(argv[1])];
	pthread_t wtid[atoi(argv[2])];
	// Create a pthread_attr.
	pthread_attr_t attr;

	// Try to initialize the pthread_attr.
	if (pthread_attr_init(&attr) != 0)
	{
		// Print error if it fails.
		fprintf(stderr,"pthread_attr_init(): %s.\n",strerror(errno));
		exit(-1);
	}

	// Loop through all threads.
	for  (long i = 0; i < atoi(argv[1]) || i < atoi(argv[2]); i++)
	{
		// Check if the current value of i is less than the second argument.
		if (i < atoi(argv[2]))
		{
			// If it is, try and create a writer thread.
			if(pthread_create(&wtid[i],&attr,writer,(void *)i) != 0)
			{
				// Print an error.
				fprintf(stderr,"pthread_create(): writer %ld error - %s.\n",i,strerror(errno));
				exit(-1);
			}

			// Increment read_count.
			write_count++;
		}
		// Check if the current value of i is less than the first argument.
		if (i < atoi(argv[1]))
		{
			// If it is, try and create a reader thread.
			if(pthread_create(&rtid[i],&attr,reader,(void *)i) != 0)
			{
				// Print an error.
				fprintf(stderr,"pthread_create(): reader %ld error - %s.\n",i,strerror(errno));
				exit(-1);
			}

			// Increment write_count.
			read_count++;
		}
	}

	// Loop through the rtid array.
	for (int i = 0; i < atoi(argv[1]); i++)
	{
		// Join the thread at the current value of rtid.
		if(pthread_join(rtid[i],NULL) != 0)
		{
			// Print an error on fail.
			fprintf(stderr,"pthread_join(): reader %d error - %s.\n",i,strerror(errno));
			exit(-1);
		}
	}
	// Loop through the wtid array.
	for (int i = 0; i < atoi(argv[2]); i++)
	{
		// Join the thread at the current value of wtid.
		if(pthread_join(wtid[i],NULL) != 0)
		{
			// Print an error on fail.
			fprintf(stderr,"pthread_join(): writer %d error - %s.\n",i,strerror(errno));
			exit(-1);
		}
	}

	// Tell the user that the threads are done.
	printf("All threads are done.\n");
}

/**************************************************************************

Function:	cleanup()

Use:		Cleans up the memory.

Arguments:	None.

Returns:	Nothing.

**************************************************************************/

void cleanup()
{
	// Try and destroy the reader/writer semaphore.
	if(sem_destroy(&write_sem) != 0)
	{
		// Print an error.
		fprintf(stderr,"sem_destroy(): reader/writer semaphore - %s.\n",strerror(errno));
		exit(-1);
	}

	// Try and destroy the critical section semaphore.
	if(sem_destroy(&read_sem) != 0)
	{
		// Print an error.
		fprintf(stderr,"sem_destroy(): critical section semaphore - %s.\n",strerror(errno));
		exit(-1);
	}

	// Tell the suer that resources are cleaned up.
	printf("Resources cleaned up.\n");
}

/**************************************************************************

Function:	main()

Use:		Checks arguments, intiializes variables, Creates reader
		and writer threads, then cleans them up.

Arguments:	None.

Returns:	Nothing.

**************************************************************************/

int main(int argc, char *argv[])
{
	// Check the arguments.
	check_args(argc, argv);

	// Initialize the variables.
	init_vars();

	// Create the threads.
	create_rws(argv);

	// Cleanup the program.
	cleanup();

	// Exit successfully.
	exit(0);
}
