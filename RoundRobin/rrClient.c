/****************************rrClient.c***************************************** 
*
* Programmer:  Tatyana Royer 
* 
* Course:  CSCI 4354
* 
* Date:  November 7, 2017
*
* Purpose: To send jobs with a given CPU burst request to a server and receive and display 
*		   the results of a round robin scheudling scheme.
*
* Input:  An int value for burst request and an int value for arrival time are made by the user.
*		  Int values for clock time and turnaround time are received from server.
* 
* Preconditions:  Only valid integers and strings will be passed in. Also, the arrival time of each client
*				  is sent in increasing order.
* 
* Output:  Program will show the time the job ended as well as its turnaround time.
* 
* Postconditions:
*
* Algorithm:
*	Create variables for file descriptors 
*	Declare instances of structs needed to send and receive data to and from the server
*	Assign a unique Private FIFO name by getting the process id
*	Prompt user for arrival time
*   Prompt user for burst integer
*	Create Private FIFO
*	Open write path to FIFO_to_Server
*	Send struct containing burst, privateFIFO, and arrival time
*	Close write path to FIFO_to_Server
*	Open read path from Private FIFO
*	Read the result struct from the Private FIFO
*	Display results
*	Close read path from Private FIFO
*	Unlink Private FIFO
*
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include "rrcommon.h"

main (void) {
	
	int fda;	// to write to server
	int fdb;	// to read response from server
	char temp[14]; // to temporarily work with the private FIFO name
	ServerData serverdata;
	ClientData clientdata;
	
	// assign the process ID as the client ID
	int clientID = getpid();
	strcpy(clientdata.privateFIFO, "FIFO_"); // get name of private FIFO
	sprintf(temp, "%d", clientID); // temp will hold the formatted clientID number
	strcat(clientdata.privateFIFO, temp); // concatenate "FIFO_" and the clientID
	//printf("\nFIFO name is %s ", jobdata.privateFIFO);
	
	// prompt user for integer inputs
	printf("\nArrival time must be an integer greater than or equal to 0 and less than 50.");
	ARRIVAL: printf("\nPlease enter arrival time for the job: ");
	scanf("%d", &clientdata.arrivalTime);
	if (clientdata.arrivalTime < 0 || clientdata.arrivalTime > 50) {
		goto ARRIVAL;
	}
	
	printf("\nThe burst request must be a positive integer less than or equal to 50.");
	BURST: printf("\nPlease enter burst request for the job: ");
	scanf("%d", &clientdata.burst);
	if (clientdata.burst < 1 || clientdata.burst > 50) {
		goto BURST;
	}
	
	printf("\nThe job will arrive at time %d and the CPU burst request is for %d units. \n", clientdata.arrivalTime, clientdata.burst);
	
	// Create the private FIFO
	if ((mkfifo(clientdata.privateFIFO,0666)<0 && errno != EEXIST))
	{
		perror("\ncan't create private FIFO");
		exit(-1);
	}

	// open up a write path to the common FIFO
	if((fda=open("FIFO_to_server", O_WRONLY))<0)
	printf("\ncan't open fifo to write");

	// write the struct to the common FIFO
	write(fda, &clientdata, sizeof(clientdata));

	// close the write path to the common FIFO
	close(fda);
	
	// open read path from private FIFO
	if((fdb=open(clientdata.privateFIFO, O_RDONLY))<0)
	printf("\ncan't open private fifo to read");

	// read the result from the private FIFO and print the result
	if(read(fdb, &serverdata, sizeof(serverdata))<0) {
		perror("read error from private FIFO");
	}
	
	printf("\nThe process completed at time %d.", serverdata.clock);
	printf("\nThe turnaround time was %d units.", serverdata.turnaround);
		
	// close the read path from the private FIFO
	close(fdb);

	// unlink the private FIFO
	unlink(clientdata.privateFIFO);

	// job finished
	printf ("\nJob Complete!\n");
	
}

