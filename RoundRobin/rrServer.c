/****************************rrServer.c***************************************** 
*
* Programmer:  Tatyana Royer 
* 
* Course:  CSCI 4354
* 
* Date:  November 7, 2017
*
* Purpose: To process jobs with a given CPU burst request using a round robin scheduling scheme.
* 
* Input:  An int value for number of clients that willl be sending burst requests and an int representing
*		  the time quantum of the round robin scheme are made by the user. A struct is sent from the
*		  client that includes a character array for privateFIFO name, an int for burst, and and int for 
*		  arrival time. 
* 
* Preconditions:  Only valid integers and strings will be passed in. Also, the arrival time of each client
*				  is sent in increasing order.
* 
* Output:  Program will show the status of the queue periodically, as well as nodes that have completed
*		   processing, and will send completion time and turnaround time to the client, and will display
*		   the average turnaround time for all the jobs.
* 
* Postconditions:  
*
* Algorithm:
*	Create ints for file descriptors, clock, and timeQuant
*	Declare instances of structs that will be sent to and from the server
*	Prompt user for number of clients
*   Prompt user for timeQuant
*	Create arrays for arrivalTime, privateFIFO, and burst, with length of numclients 
*	Create the Ready queue used to store job nodes
*	Initialize clock to 0
*	Create the FIFO_to_Server
*	Open a read path from FIFO_to_Server 
*	For each client
*		If the client has sent data to the FIFO_to_Server
*			Read the arrival time, private FIFO name, and burst from FIFO_to_Server
*		Else if data has not been sent
*			Continue to try to read from the FIFO_to_Server until data has been sent
*		Print the data that was sent to the server  
*		Store the data in the arrays
*		Open up a write path to the Private FIFO
*	For each element in arrivalTime
*		Create a node for the job with earliest arrivalTime
*		Enqueue the node to Ready
*	While Ready is not empty
*		Display contents of queue if a node has been dequeued
*		If head node has not yet arrived and there is another job waiting to be processed
*			Send head node to back of queue
*		If head node has not yet arrived and there are no jobs waiting to be processed
*			Set clock to head node's arrival time
*		If timeQuant is greater than burst of the node at the head of the queue
*			Clock is incremented by burst
*			Head node is dequeued
*			Burst is set to 0
*			Calculate turnaround
*			If queue is not empty
*				Shift the arrivalTime array appropriately
*			Open write path to private FIFO
*			Send turnaround and clock value to private FIFO
*			Close write path to private FIFO
*		Else
*			Clock is incremented by timeQuant
*			Burst is set to burst minus timeQuant
*			Head node is enqueued to the tail
*	Close the read path from FIFO_to_Server
*	Unlink FIFO_to_Server
*	Display the average turnaround time
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

// Had issues including queue code as a header file, so put it in the program itself
// Queue Code

typedef struct{
	char privateFIFO[14];
	int jobnum;
	int burst;
	int arrivalTime;
	int mark;
} Object;

typedef struct node{  /*Nodes stored in the linked list*/
	Object element;
	struct node *next;
} Node;

typedef struct queue{ /*A struct facilitates passing a queue as an argument*/
	Node *head;       /*Pointer to the first node holding the queue's data*/
	Node *tail;       /*Pointer to the last node holding the queue's data*/
	int sz;           /*Number of nodes in the queue*/
} Queue;

int size( Queue *Q ){
	return Q->sz;
}

int isEmpty( Queue *Q ){
	if( Q->sz == 0 ) return 1;
	return 0;
}

void enqueue( Queue *Q, Object elem ){
	Node *v = (Node*)malloc(sizeof(Node));/*Allocate memory for the Node*/
	if( !v ){
		printf("ERROR: Insufficient memory\n");
		return;
	}
	v->element = elem;
	v->next = NULL;
	if( isEmpty(Q) ) Q->head = v;
	else Q->tail->next = v;
	Q->tail = v;
	Q->sz++;
}

Object dequeue( Queue *Q ){
	Node *oldHead;
	Object temp;
	if( isEmpty(Q) ){
		printf("ERROR: Queue is empty\n");
		return temp;
	}
	oldHead = Q->head;
	temp = Q->head->element;
	Q->head = Q->head->next;
	free(oldHead);
	Q->sz--;
	return temp;
}

Object first( Queue *Q ){
	if( isEmpty(Q) ){
		Object temp;
		printf("ERROR: Queue is empty\n");
		return temp;
	}
	return Q->head->element;
}

void destroyQueue( Queue *Q ){
	while( !isEmpty(Q) ) dequeue(Q);
}

/*A different visit function must be used for each different datatype.*/
/*The name of the appropriate visit function is passed as an argument */
/*to traverseQueue.                                                   */
void visitInt( Object elem ){
	printf("\nJob %d ",elem.jobnum);
}
/*The following function isn't part of the Queue ADT, however*/
/*it can be useful for debugging.                            */
void traverseQueue( Queue *Q, void (*visit)(Object elem) ){
	printf("\nThe cue currently consists of: ");
	Node *current = Q->head;
	while( current ){
		visit(current->element);
		current = current->next;
	}
	printf("\n");
}

// end Queue Code 


// Begin Main Server Program
int main( int argc, int *argv[] ){

	// initialization
	int clock;
	int timeQuant;
	int numclients;
	int finish;
	int fda;		// for the FIFO_to_Server
	int length;	
	int waiting = 0;	// a flag to show if another job is waiting to be run
	int markCount = 0;	// used to cycle through the queue correctly
	ServerData serverdata;	// instantiate the struct that will send data from server to client
	ClientData clientdata;  // instantiate the struct that will receive data from client to server
	
	// Prompt user for number of clients
	CLIENTS: printf("\nPlease enter how many clients will be sending memory requests (choose an integer between 3 and 10): ");
	scanf("%d", &numclients);
	if (numclients < 3 || numclients > 10) {
		goto CLIENTS;
	}
	
	// Prompt user for time quantum
	TIME: printf("\nPlease enter a value for the time quantum (choose an integer between 5 and 15): ");
	scanf("%d", &timeQuant);
	if (timeQuant < 5 || timeQuant > 15) {
		goto TIME;
	}
	
	printf("\n%d clients will be created.\n", numclients);
	printf("\nThe time quantum chosen is %d units.\n", timeQuant);
	
	// create necessary arrays to store data for each client
	char FIFOs[numclients][14];
	int arrivalTime[numclients];
	int burst[numclients];
	int turnaround[numclients];
	int fdb[numclients];
	
	// used to calculate arrival time of next job in queue
	length = sizeof(arrivalTime)/sizeof(int);
	
	// create and initialize the Ready queue
	Queue Ready;
	Ready.head = NULL;
	Ready.tail = NULL;
	Ready.sz = 0;
	
	// Create the common FIFO
	if ((mkfifo("FIFO_to_server",0666) < 0 && errno != EEXIST)) {
		perror("cant create FIFO_to_server");
		exit(-1);
	}
	
	// Open a read path from the common FIFO
	if( (fda = open("FIFO_to_server", O_RDONLY) ) < 0) {
		printf("cant open fifo to read");
	}

	int i = 0;
	// for each client
	for (i; i<numclients; i++){
		// read the struct from the common FIFO and check for read errors
		READ: if ((finish = read(fda, &clientdata, sizeof(clientdata))) == -1) {
			perror("read error");
		}
		
		// we read no data so go to the next read
		if (finish == 0) {
			goto READ;
		}
		
		// print data that was sent from the client
		printf("\nJob %d has an arrival time of %d and burst of %d. ", i, clientdata.arrivalTime, clientdata.burst);
		
		// store the information in the arrays
		strcpy(FIFOs[i], clientdata.privateFIFO); 
		arrivalTime[i] = clientdata.arrivalTime;
		burst[i] = clientdata.burst;
		
		// private FIFO was created on the client side
		// open a write path to the private FIFO
		if((fdb[i]=open(clientdata.privateFIFO, O_WRONLY))<0) {
			printf("\ncan't open fifo to write");
		}
	}
	
	// create a node for each job and enqueue it to Ready
	i = 0;
	Object object;
	for (i; i<numclients; i++) {
		object.jobnum = i;
		strcpy(object.privateFIFO, FIFOs[i]);
		object.arrivalTime = arrivalTime[i];
		object.burst = burst[i];
		enqueue(&Ready, object);
	}
	
	// function to print node that was dequeued
	void printDequeued(Object object) {
		printf("\n********Dequeued Node*****************");
		printf("\nJob number = %d", object.jobnum);
		printf("\nPrivateFIFO = %s", object.privateFIFO);
		printf("\nBurst = %d", object.burst);
		printf("\nArrival Time = %d\n", object.arrivalTime);
		printf("\n**************************************");
	}
	
	// set a counter to the size of the queue that will be used to periodically
	// display the contents of the queue
	int counter = size(&Ready);
	
	// clock is the earliest arrival time
	clock = arrivalTime[0];
	
	// print the queue initially before the process starts
	printf("\n");
	traverseQueue(&Ready, visitInt);
	
	// while Ready queue is not empty
	WHILE: while (isEmpty(&Ready) != 1) {
		
		// print out the queue if a node has been dequeued
		if (counter > size(&Ready)) {
			traverseQueue(&Ready, visitInt);
			counter--;
		} 
		
		// work with the head of the queue
		Object current = first(&Ready);
		
		// if current node has not yet arrived and there is another job waiting to be processed,
		// send it to the back of the queue
		if (current.arrivalTime > clock && waiting == 1) {
			current = dequeue(&Ready);
			enqueue(&Ready, current);
			goto WHILE;
		}
		// if current node has not yet arrived and no other job is waiting to be processed, set the
		// clock to the arrival time and begin processing the job
		else if (current.arrivalTime > clock && waiting == 0) {
			clock = current.arrivalTime;
		}
		
		// if current node's burst time remaining is less than or equal to the timeQuant value,
		// process and dequeue the node and send the result back to the client
		PROCESSING: if (timeQuant >= current.burst) {
			
			// if more than one node in the queue is marked
			if (markCount > 1) {
				// if the current node to be dequeued is marked, decrement the mark count
				if (current.mark == 1) {
					markCount--;
				}
			}
			// if only one node in the queue is marked
			else if (markCount == 1) {
				// and the current node is it, then decrement the mark count and take away the 
				// waiting flag
				if (current.mark == 1) {
					markCount--;
					waiting = 0;
				}
			}
			
			current = dequeue(&Ready);	// dequeue the node
			clock = clock + current.burst;	// update the clock value
			current.burst = 0;			// no more time remaining in burst
			serverdata.clock = clock;	// assign the clock value to the struct
			serverdata.turnaround = clock - current.arrivalTime; // assign turnaround to struct
			turnaround[current.jobnum] = serverdata.turnaround;	// add turnaround to array to be averaged later
			printDequeued(current);		// print the dequeued node
			
			// if there is more than one node remaining to be processed in the queue,
			// shift the arrivalTime array and shorten it accordingly
			if (size(&Ready) > 1) {
				i = current.jobnum;
				while (i<length) {
					arrivalTime[i] = arrivalTime[i+1];
					length--;
					i++;
				}
			}
			// if only one node is remaining, shorten it accordingly
			else if (size(&Ready) == 1){
				i = current.jobnum;
				arrivalTime[i] = arrivalTime[i+1];
				length--;
			}
		
			// write the result to the private FIFO
			write(fdb[current.jobnum], &serverdata, sizeof(serverdata));
			printf("\nSent the result to the client.");
		
			// close the write path to the private FIFO
			close(fdb[current.jobnum]);
			printf("\nClosed write path to the private FIFO.\n");
		}
		// if current node's burst time remaining is greater than timeQuant value, update the burst
		// and clock values and send the node to the back of the queue
		else {
			clock = clock + timeQuant;
			current.burst = current.burst - timeQuant;
			
			// if the size of the queue is greater than 1
			if (size(&Ready) > 1) {
				// if the next job has not yet arrived and there are no other nodes waiting to
				// be processed, continue to work with current job
				if (arrivalTime[current.jobnum + 1] > clock && waiting != 1) {
					goto PROCESSING;
				}
				// otherwise go to the next node
				else {
					// if current node is marked, send it to the back of queue
					if (current.mark == 1) {
						waiting = 1;
						int thisburst = current.burst;
						current = dequeue(&Ready);
						current.burst = thisburst;
						enqueue(&Ready, current);
					}
					// if it isn't marked, mark it, increment the mark count, set the waiting flag,
					// and send it to the back of the queue
					else {
						waiting = 1;
						int thisburst = current.burst;
						current = dequeue(&Ready);
						current.burst = thisburst;
						current.mark = 1;
						markCount++;
						enqueue(&Ready, current);
					}
				}
			}
			// if size of queue is not greater than 1, just enqueue and dequeue
			else {
				int thisburst = current.burst;
				current = dequeue(&Ready);
				current.burst = thisburst;
				enqueue(&Ready, current);
			}
		}
	}
	
	// close the read path from the common FIFO
	close(fda);
	printf("\nClosed read path from common FIFO.");

	// unlink the common FIFO
	unlink("FIFO_to_server");
	printf("\nUnlinked the common FIFO.\n");
	
	// display the average turnaround time
	int sum = 0;
	i = 0;
	for (i; i<numclients; i++) {
		sum = sum + turnaround[i];
	}
	int ave = sum / numclients;
	printf("\n*****************************************");
	printf("\nThe average turnaround time was %d units.", ave);
	printf("\n*****************************************\n");
		
}




