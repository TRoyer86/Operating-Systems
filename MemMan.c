/****************************MemMan.c***************************************** 
*
* Programmer:  Tatyana Royer 
* 
* Course:  CSCI 4354 
* 
* Date:  September 25, 2017
* 
* Assignment:  Program #1: Memory Management 
* 
* Environment:  Windows 10, Linux via putty, vi editor, Notepad++
* 
* Files Included: memman.c, ReadMe, output1.docx, output2.docx
* 
* Estimated Time Spent: design (1 hr), implementation(1 hr), testing(1 hr)
*
* Actual Time Spent: design (2 hr), implementation(3 hr), testing(1 hr)
*
* Purpose: To take in integer values representing memory requests and to use
*		   both First Fit and Best Fit memory allocation schemes in order to
*		   assign the requests to partitions. 
* 
* Input:  An integer array representing the sizes of 3 partitions
*		  and an integer array representing  5 memory requests of various sizes.
*		  These numbers were chosen purposefully to be easy to check and to have more
*		  requests than available partitions.
* 
* Preconditions:  Only integers can be given as input. The user can choose either
*				  First Fit or Best Fit for each run. 
* 
* Output:  For each memory request assigned to a partition, the output will consist of
*		   the job number, the starting address, the ending address, its internal fragmentation,
*		   and the total fragmentation as the program runs. For each memory request too
*		   large to be assigned an available partition, the job number will be printed out
*		   as well as the reason it was unassigned. For each memory request that cannot be 
*		   assigned a partition because there are no free partitions, the job number and the
*		   reason it was unassigned will be printed.
* 
* Postconditions:  
* 
* Algorithm: 
*	Ask user to choose first fit (f) or best fit (b)
*		If f is chosen
*			// set up steps
*			Create int arrays for paritions, requests, and start address
*			Create ints for the length of each array and for the program's total fragmentation
*			Prompt user to enter 3 partition sizes and 5 memory requests
*			Populate the partitions and requests array
*			Calculate the start addresses for each partition and populate the start address array
*			// first fit algorithm
*			For each memory request
*				While there are available partitions
*					Compare the memory request with the size of the current partition
*						If the memory request can fit into the parition
*							Calculate the memory request's starting address, ending address, internal fragmentation, and total fragmentation
*							Shift the elements in the partitions and start address arrays down
*							Decrement the partitions and start address arrays
*							Print the job number, start address, end address, internal frag, and total frag
*						If the memory request cannot fit into the partition 
*							Try the next available partition
*					If all available partitions cannot fit the memory request
*						Print that the job was too large to fit into a partition
*				If there are no available partitions
*					Print that the job was unassigned because there were no available partitions
*					
*		If b is chosen
*			// set up steps
*			Create int arrays for paritions, requests, and start address
*			Create ints for the length of each array and for the program's total fragmentation
*			Prompt user to enter 3 partition sizes and 5 memory requests
*			Populate the partitions and requests array
*			Calculate the start addresses for each partition and populate the start address array
*			// best fit algorithm
*			For each memory request
*				While there are available partitions
*					Create an int best to store the index of the best fit partition and initialize it to a junk value
*					Compare the memory request with the size of the current partition
*						If memory request can fit into the partition
*							If best is a junk value
*								Set best to the index of the current partition
*							Else if the current partition is less than the best partition
*								Set best to the index of the current partition
*						If memory request cannot fit into the partition
*							Try the next available partition
*					If all available partitions cannot fit the memory request
*						Print that the job was too large to fit into a partition
*					Calculate the memory request's starting address, ending address, internal fragmentation, and total fragmentation using best as the index
*							Shift the elements in the partitions and start address arrays down
*							Decrement the partitions and start address arrays
*							Print the job number, start address, end address, internal frag, and total frag
*				If there are no available partitions
*					Print that the job was unassigned because there were no available partitions
***********************************************************************/ 

#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/errno.h>
#include<fcntl.h>

main(void)
{
	// introductory statement
	printf("************************************************************************************\n");
	printf("This program demonstrates the first fit and best fit methods of memory allocation.\n");

	printf("To demonstrate first fit, type (f/F). For best fit, type (b/B).\n");
	char response = getchar();
	if(response == 'f' || response == 'F') // if first fit is chosen
	{
		int partitions[3];	// create array of three partitions, where each element is the size of the partition
		int requests[5];	// create array of memory requests, where each element is the size of the request
		int startadd[3];	// create array of start addresses, which will be calculated from the partitions array
		int partlength = sizeof(partitions)/sizeof(int);
		int reqlength = sizeof(requests)/sizeof(int);
		int addlength = sizeof(startadd)/sizeof(int);
		int totalfrag = 0; // initialize the total fragmentation to 0

		// introductory statement
		printf("*********************************************************************\n");
		printf("The following demonstrates the first fit method of memory allocation.\n");

		// get elements for the partitions array
		printf("Enter 3 partition sizes for the partitions array:\n");
		int i = 0;
		for(i=0;i<3;i++)
		{
			scanf("%d",&partitions[i]); // takes the input and assigns them to partitions array
		}

		// get elements for the requests array
		printf("Enter 5 memory request amounts for the requests array:\n");
		for(i=0;i<5;i++)
		{
			scanf("%d",&requests[i]);
		}

		// calculate and populate the start address array
		startadd[0] = 0;	// first starting address is 0
		for(i=1; i<3; i++)
		{
			startadd[i] = startadd[i-1] + partitions[i-1]; // calculate the start address of each partition
		}

		// first fit method algorithm
		i = 0;
		FIRSTOUTER: while(i<reqlength) // for each memory request
		{
			if(partlength > 0) // if there are available partitions
			{
				int j = 0;
				while(j<partlength) // iterate through the available partitions until found
				{
					if(requests[i] <= partitions[j]) // if the memory request can fit in the partition
					{	
						int start = startadd[j]; // set the start address
						int end = start + (requests[i] - 1); // set end address
						int frag = partitions[j] - requests[i]; // calculate fragmentation
						totalfrag = totalfrag + frag; // update total fragmentation
						
						// set temporary variables to work with the arrays to be shifted
						int k = j; 
						int temppartlength = partlength;
						int tempaddlength = addlength;
						while(k<temppartlength) // update arrays
						{
							partitions[k] = partitions[k+1]; // update partitions array
							startadd[k] = startadd[k+1]; // update startadd array
							temppartlength--;
							tempaddlength--;
							k++;
						}
						partlength--;
						addlength--;
						
						printf("Job number: %d\n",i);
						printf("Start address: %d\n",start);
						printf("End address: %d\n",end);
						printf("Internal fragmentation: %d\n",frag);
						printf("Total fragmentation: %d\n",totalfrag);
						i++;	// increment i
						goto FIRSTOUTER;
					}
					else 
					{
						j++; // go to next partition
					}
					
				}
				printf("Job %d is too large to be assigned to available partitions.\n", i);
				i++; // go to the next request		
			}
			else
			{
				printf("Job %d is unassigned because no partitions are available.\n", i);
				i++;
			}
			
		}
	}
	else if(response == 'b' || response == 'B') // if best fit is chosen
	{
		int partitions[3];	// create array of six partitions, where each element is the size of the partition
		int requests[5];	// create array of memory requests, where each element is the size of the request
		int startadd[3];	// create array of start addresses, which will be calculated from the partitions array
		int partlength = sizeof(partitions)/sizeof(int);
		int reqlength = sizeof(requests)/sizeof(int);
		int addlength = sizeof(startadd)/sizeof(int);
		int totalfrag = 0; // initialize the total fragmentation to 0

		// introductory statement
		printf("********************************************************************\n");
		printf("The following demonstrates the best fit method of memory allocation.\n");

		// get elements for the partitions array
		int i =0;
		printf("Enter 3 partition sizes for the partitions array:\n");
		for(i=0;i<3;i++)
		{
			scanf("%d",&partitions[i]); // takes the input and assigns them to partitions array
		}

		// get elements for the requests array
		printf("Enter 5 memory request amounts for the requests array:\n");
		for(i=0;i<5;i++)
		{
			scanf("%d",&requests[i]);
		}

		// calculate and populate the start address array
		startadd[0] = 0;	// first starting address is 0
		for(i=1; i<3; i++)
		{
			startadd[i] = startadd[i-1] + partitions[i-1]; // calculate the start address of each partition
		}

		// best fit method algorithm
		i = 0;
		BESTOUTER: while(i<reqlength) // for each memory request
		{
			if(partlength > 0) // if there are available partitions
			{
				int j = 0;
				int best = -1; // initialize a junk index value
				while(j<partlength) // iterate through the available partitions until found
				{
					if(requests[i] <= partitions[j]) // if the memory request can fit in the partition
					{
						if(best == -1)
						{
							best = j;	// the best fit initially is the first partition in which the request can be loaded into
						}
						else if(partitions[j] < partitions[best]) // compare the sizes of valid partitions to find the best fit
						{
							best = j;	// update the best position
						}
						j++; // increment j to test the next partition
					}
					else // if the memory request can't fit into the partition
					{
						j++; // increment to the next partition
					}
				}

				if(best == -1) // if there was no partition large enough for the job
				{
					printf("Job %d is too large to be assigned to available partitions.\n", i);
					i++; // go to the next request
					goto BESTOUTER;
				}
				else
				{
					int start = startadd[best]; // set the start address
					int end = start + (requests[i] - 1); // set end address
					int frag = partitions[best] - requests[i]; // calculate fragmentation
					totalfrag = totalfrag + frag; // update total fragmentation

					int k = best;
					int temppartlength = partlength;
					int tempaddlength = addlength;
					if(k != (temppartlength - 1))
					{
						while(k<temppartlength) // update arrays
						{
							partitions[k] = partitions[k+1]; // update partitions array
							startadd[k] = startadd[k+1]; // update startadd array
							temppartlength--;
							tempaddlength--;
							k++;
						}
						partlength--;
						addlength--;
					}
					else
					{
						partlength--;
						addlength--;
					}

					printf("Job number: %d\n",i);
					printf("Start address: %d\n",start);
					printf("End address: %d\n",end);
					printf("Internal fragmentation: %d\n",frag);
					printf("Total fragmentation: %d\n",totalfrag);
					i++;	// increment i
					goto BESTOUTER;
				}
			}
			else
			{
				printf("Job %d is unassigned because no partitions are available.\n", i);
				i++;
			}


		}
		
	}
	 
}