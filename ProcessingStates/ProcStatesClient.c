/****************************ProcStatesClient.c***************************************** 
*
* Programmer:  Tatyana Royer 
* 
* Course:  CSCI 4354
* 
* Date:  November 24, 2017
*
* Purpose: To send jobs with an array of alternating CPU and I/O burst requests to a server and receive and display 
*		   the results of the processing of the CPU bursts with a round robin scheme and the I/O bursts with abort
*		   first come first server scheme.
*
* Input:  An int value for number of bursts request and int values for those bursts are made by the user.
*		  Int values for clock time and utilization time are received from server.
* 
* Preconditions:  Only valid integers and strings will be passed in. The first and final bursts are CPU bursts, which
*				  alternate with I/O bursts and there must be an odd number of total bursts. This program will accept
*				  a total number of bursts in the range of 1 to 9. Burst requests can range from 1 to 100. Job arrives
*				  at time 0.
* 
* Output:  Program will show the time the job ended as well as the percentage of time it spent processing both
*		   I/O and CPU bursts.
* 
* Postconditions:
*
* Algorithm:
*	Create variables for file descriptors , number of bursts, and burst total
*	Declare instances of structs needed to send and receive data to and from the server
*	Prompt user for number of bursts
*	Assign a unique Private FIFO name by getting the process id
*   Prompt user for values of bursts and store in an array
*	Set dummy values for unused portion of the bursts array
*	Create Private FIFO
*	Open write path to FIFO_to_Server
*	Send struct privateFIFO, bursts array, and sum of the bursts to the server via FIFO_to_Server
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

#include "pscommon.h"

void main (void) {

int fda;	// to write to server
int fdb;	// to read response from server
int numOfBursts; // to get number of bursts
float totalBursts = 0;
char temp[14]; // to temporarily work with the private FIFO name
ClientData clientdata;
ServerData serverdata;

// prompt for the number of CPU and I/O bursts
printf("\nEach client will send the server an array of alternating CPU and I/O bursts.");
printf("\nThe array will be an odd number length, with the first and final elements being CPU bursts.");
BURSTNUM: printf("\nHow many bursts would you like to process? (must be an odd number less than 10): ");
scanf("%d", &numOfBursts);
if ((numOfBursts % 2) == 0 || numOfBursts > 10 || numOfBursts < 0) {
	goto BURSTNUM;
}

// assign the process ID as the client ID
int clientID = getpid();
strcpy(clientdata.privateFIFO, "FIFO_"); // get name of private FIFO
sprintf(temp, "%d", clientID); // temp will hold the formatted clientID number 
strcat(clientdata.privateFIFO, temp); // concatenate "FIFO_" and the clientID
//printf("\nFIFO name is %s ", jobdata.privateFIFO);

// prompt user for integer inputs for the bursts array
int i;
BURSTS: printf("Enter the bursts (must be between 1 and 100):");
for(i=0;i<numOfBursts;i++)
{
	scanf("%d",&clientdata.bursts[i]);
	if ((clientdata.bursts[i]) < 1 || clientdata.bursts[i] > 100) {
		goto BURSTS;
	}
}

for(i=0;i<numOfBursts;i++)
{
	totalBursts = totalBursts + clientdata.bursts[i];
}

printf("\nThe burst total is %.0f.\n", totalBursts);
clientdata.totalBursts = totalBursts;

// set dummy values to unused portions of the burst array
for(i=numOfBursts;i<10;i++) {
	clientdata.bursts[i] = -1;
}

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

//printf("\nThe process completed at time %f.", serverdata.clock);
printf("\nThe job was actively processing %.2f percent of the time since its arrival.", serverdata.utilization);
float myclock = serverdata.clock;
printf("\nThe process completed at time %.0f.\n", myclock);
	
// close the read path from the private FIFO
close(fdb);

// unlink the private FIFO
unlink(clientdata.privateFIFO);

// job finished
printf ("\nJob Complete!\n");

}

