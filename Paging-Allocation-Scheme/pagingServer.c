/****************************privateServer.c***************************************** 
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
* Input:  Input will be values from a struct passed from the client, one string and
* 	      two integers.
* 
* Preconditions:  Only valid integers and strings will be passed in.
* 
* Output:  An integer result that is the difference of the two numbers passed in.
* 
* Postconditions:  
* 
* Algorithm: 
*	Create the common FIFO
*	Open a read path from the common FIFO
*   Read the input from the common FIFO
*   Display the input that was read
*   Calculte the difference between num1 and num2 and store the value in result.
*   Open a write path to the private FIFO
*	Write the result to the private FIFO
*   Close the write path to the private FIFO
*   Close the read path from the common FIFO
*   Unlink the common FIFO
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

#include "common.h" // header file that houses the struct we are using

int main (void) 
{
int fda;      // for the FIFO_to_Server
int fdb;      // for each private FIFO
int finish;   // lets me know that client is done
int const pagesize = 256; // page size
int numclients; // number of clients
int numframes; // number of frames in main memory
int remainingframes; // remaining frames after each allocation
int startingpage = 0; // first page of memory initialized to 0
JobData jobdata; // create the struct to receive info from client
ResultData resultdata; // create the struct to send result to client



// prompt user for integer inputs
printf("\nThis program demonstrates a paging memory allocation scheme.");
printf("\nEach page and frame will represent %d bytes of memory.", pagesize);
printf("\nThe client will send over a job request of a specific size to the server.");
printf("\nThe server will determine how many frames the job requires and allocate memory for the jobs.");
printf("\nPlease enter how many clients will be sending memory requests (choose an integer between 3 and 6): ");
scanf("%d", &numclients);
printf("Please enter how many frames will exist in main memory (choose an integer between 12 and 24): ");
scanf("%d", &numframes);
printf("\n%d clients will be created.", numclients);
printf("\nMain memory consists of %d frames of %d bytes each.", numframes, pagesize);
remainingframes = numframes;

// create a struct for the final memory map
typedef struct {
	const char *jobnames[numclients];
	int startpage[numclients];
	int endpage[numclients];
	int endbyte[numclients];
	int frag[numclients];
} Memory;

Memory memory; // declare the memory struct

int i = 0;

// Create the common FIFO
if ((mkfifo("FIFO_to_server",0666) < 0 && errno != EEXIST)) {
	perror("cant create FIFO_to_server");
	exit(-1);
}

// Open a read path from the common FIFO
if( (fda = open("FIFO_to_server", O_RDONLY) ) < 0) {
printf("cant open fifo to read");
}

// using for error checking
printf("\nCommon FIFO fda is %d", fda);

// read the struct from the common FIFO and check for read errors
if ((finish = read(fda, &jobdata, sizeof(jobdata))) == -1) {
perror("read error");
}

// these lines show what was read from the common FIFO and the result
printf("\nFIFO name is %s ", jobdata.privateFIFO);
printf("\nJob name is %s ", jobdata.jobname);
printf("\nRequested memory is %d bytes.", jobdata.memrequest);


// calculate the number of frames needed, fragmentation, displacement, and bytes used
double floor;
int frag; // fragmentation
int framesneeded; // how many frames will the job need
int displacement; // remainder
int bytesused; // how many bytes used in the last frame
floor = jobdata.memrequest / pagesize; 
displacement = jobdata.memrequest % pagesize;
if (displacement == 0) {
	framesneeded = floor;
	frag = 0;
}
else {
	framesneeded = floor + 1;
	bytesused = displacement + 1;
	frag = pagesize - bytesused;
}

// storing the results
// if there are not enough frames to allocate memory for the request
if (framesneeded > remainingframes) {
	memory.startpage[i] = -1; // assign negative values to elements in arrays to represent not needed elements
	memory.endpage[i] = -1;
	memory.endbyte[i] = -1;
	memory.frag[i] = -1;
	memory.jobnames[i] = jobdata.jobname;
	
	// store the results to send back to client
	strcpy(resultdata.msg, "Request too large to be allocated in memory.");
	resultdata.frag = -1;
}

// if the job can be allocated to memory
else if (framesneeded <= remainingframes) {
memory.startpage[i] = startingpage; // store the starting page of the job
memory.endpage[i] = framesneeded - 1; // store the ending page of the job
startingpage = startingpage + framesneeded; // get the starting page for the next job
memory.endbyte[i] = displacement; // the ending byte is the displacement
memory.frag[i] = frag; // assign fragmentation
memory.jobnames[i] = jobdata.jobname; // assign job name
remainingframes = remainingframes - framesneeded; // decrement the number of remaining frames in memory

// store the results in the struct to send back to client
resultdata.frag = frag;
resultdata.startframe = memory.startpage[i];
resultdata.endframe = memory.endpage[i];
}

// private FIFO was created on the client side
// open a write path to the private FIFO
if((fdb=open(jobdata.privateFIFO, O_WRONLY))<0) {
   printf("cant open fifo to write");
}
// using for error checking
printf("\nPrivate FIFO fdb is %d", fdb);

// write the result to the private FIFO
write(fdb, &resultdata, sizeof(resultdata));
printf("\nSent the result to the client.\n");
/*
// close the write path to the private FIFO
close(fdb);

// close the read path from the common FIFO
close(fda);
printf("Closed path from common FIFO.\n");

// unlink the common FIFO
unlink("FIFO_to_server");
printf("Unlinked common FIFO.\n");
*/
// print out the final memory map
printf("\n%s: ", memory.jobnames[i]);
printf("\nStart Frame: %d. Start Byte: 0. End Frame: %d. End Byte: %d", memory.startpage[i], memory.endpage[i], memory.endbyte[i]);
printf("\nFragmentation: %d", memory.frag[i]);

}