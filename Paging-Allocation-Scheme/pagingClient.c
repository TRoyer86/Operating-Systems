/****************************privateClient.c***************************************** 
*
* Programmer:  Tatyana Royer 
* 
* Course:  CSCI 4354
* 
* Date:  October 12, 2017
* 
* Assignment: Homework 6 
* 
* Environment:  Windows 10, linux via putty, Notepad++, vi editor
* 
* Files Included: privateServer.c, privateClient.c, common.h
*
* Purpose: To allow the creation of a private FIFO between the server and client.
* 
* Input:  Input will be a string generated from the process ID and two integers
*         given by the user.
* 
* Preconditions:  Only valid integers and strings will be passed in.
* 
* Output:  An integer result that is the difference of the two numbers passed in.
* 
* Postconditions:  
* 
* Algorithm: 
*	Generate the private FIFO name from the procecss ID
*   Prompt user for integer inputs
*	Create the private FIFO
*   Open a write path to the common FIFO
*   Write the struct to the common FIFO
*   Close the write path to the common FIFO
*   Open a read path from the private FIFO
*   Read the result from the private FIFO
*   Close the read path from the private FIFO
*   Unlink the private FIFO
*
***********************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include "common.h"

main (void)
{
int fda;	// to write to server
int fdb;	// to read response from server
char temp[14]; // to temporarily work with the private FIFO name
JobData jobdata; // create the struct to receive info from client
ResultData resultdata; // create the struct to send result to client

// assign the process ID as the client ID
int clientID = getpid();
strcpy(jobdata.privateFIFO, "FIFO_"); // get name of private FIFO
sprintf(temp, "%d", clientID); // temp will hold the formatted clientID number
strcat(jobdata.privateFIFO, temp); // concatenate "FIFO_" and the clientID
printf("\nFIFO name is %s ", jobdata.privateFIFO);

// prompt user for integer inputs
printf("\nPlease name the job request (20 characters or less): ");
scanf("%s", &jobdata.jobname);
printf("How many bytes of memory are requested? (Pick a positive integer less than 6000): ");
scanf("%d", &jobdata.memrequest);
printf("\n%d bytes have been requested for %s ", jobdata.memrequest, jobdata.jobname);

// Create the private FIFO
if ((mkfifo(jobdata.privateFIFO,0666)<0 && errno != EEXIST))
{
perror("cant create private FIFO");
exit(-1);
}

// open up a write path to the common FIFO
if((fda=open("FIFO_to_server", O_WRONLY))<0)
	printf("cant open fifo to write");


// write the struct to the common FIFO
write(fda, &jobdata, sizeof(jobdata));

// close the write path to the common FIFO
close(fda);


// open read path from private FIFO
if((fdb=open(jobdata.privateFIFO, O_RDONLY))<0)
	printf("\ncant open private fifo to read");

printf("\nfdb is: %d", fdb);

// read the result from the private FIFO and print the result
if(read(fdb, &resultdata, sizeof(resultdata))<0) {
	perror("read error from private FIFO");
}

// if job was too large to be allocated to memory
if (resultdata.frag == -1) {
	printf("\n%s", resultdata.msg);
	//printf("Request too large to be allocated in memory.");
}
else {
	printf("\nJob name: %s", jobdata.jobname);
	printf("\nAssigned frames: %d - %d. Fragmentation: %d", resultdata.startframe, resultdata.endframe, resultdata.frag);
}
/*
// close the read path from the private FIFO
close(fdb);

// unlink the private FIFO
unlink(input.privateFIFO);

printf ("\nall done!\n");
*/
}


