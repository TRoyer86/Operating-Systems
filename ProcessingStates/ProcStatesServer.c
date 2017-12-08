/****************************ProcStatesServer.c***************************************** 
*
* Programmer:  Tatyana Royer 
* 
* Course:  CSCI 4354
* 
* Date:  November 24, 2017
*
* Purpose: To process jobs with a given CPU burst request using a round robin scheduling scheme.
* 
* Input:  An int value for number of clients that willl be sending burst is made by the user. A struct is sent from the
*		  client that includes a character array for privateFIFO name, an array of bursts, and a float value of the sum of the bursts.
* 
* Preconditions:  Only valid integers and strings will be passed in. All jobs arrive at time 0 and will be added to the Ready queue
*				  according to which client sent the job first. The round robin time quant is 5 units. Number of clients must be 
*				  between 3 and 10. Though the running state is implemented with a queue, it is only ever at size 1 or empty.
* 
* Output:  Program will show the status of the queue periodically, as well as nodes that have completed
*		   processing, and will send completion time and the percentage that job was actively being processed to the client. It
*		   will also display the CPU utilization time for the entire session at the end.
* 
* Postconditions:  
*
* Algorithm:
*	Create variables and initialize to 0 for clock, time quant counter, utilization, and idle CPU time
*	Create variables for file descriptor, number of processes, and finish
*	Declare instances of structs needed to send and receive data to and from the server
*	Prompt user for number of clients
*	Create array to store file descriptors for each client
*	Create the FIFO_to_Server
*	Open a read path from FIFO_to_Server
*	Create and initialize a queue for Ready, Running, and Waiting
*	For each client
*		Read the struct from the FIFO_to_Server
*		Create a PCB to store the job number, privateFIFO name, bursts array, and burst position 
*		Enqueue the object to Ready
*		Open a write path to its privateFIFO
*	Set number of processes to number of clients
*	While number of processes is greater than 0
*		If all PCBs are in Ready
*			Dequeue from Ready and enqueue to Running
*			Set quantCount to 1
*			Increment clock
*			Go to while Loop
*		If only one PCB remains and is in Running
*			If PCB is on final CPU burst
*				If the burst is complete
*					Dequeue from Running and send results to client
*					Decrement number of processes
*					Increment clock
*					Go to while Loop
*				Else
*					Keep running the process
*					Increment quantCount and clock
*					Go to while Loop
*			Else
*				If this CPU burst is complete
*					Dequeue from Running, update I/O, and enqueue to Waiting
*					Increment Clock
*					Reset quantCount to 0
*					Print contents of queues
*					Go to while loop
*		If PCBs are in Ready and Running but not in Waiting
*			If PCB is on final CPU burst
*				If the burst is complete
*					Dequeue from Running and send results to client
*					Increment clock
*					Decrement number of processes
*					Dequeue from Ready, update CPU, and enqueue to Running
*					Set quantCount to 1
*					Print Contents of queues
*					Go to while loop
*				Else
*					If round robin time still remains
*						Keep running the process
*						Increment quantCount and clock
*						Go to while loop
*					Else
*						Dequeue from Running and enqueue to Ready
*						Dequeue from Ready, update CPU, and enqueue to Running
*						Set quantCount to 1 and increment clock
*						Go to while loop
*			Else
*				Increment clock
*				If current burst is complete
*					Dequeue from Running, update I/O, and enqueue to Waiting
*					Dequeue from Ready, update CPU, and enqueue to Running
*					Set quantCount to 1
*					Print contents of queues
*					Go to while loop
*				Else
*					If round robin time still remains
*						Keep running the process
*						Increment quantCount
*						Go to while loop
*					Else
*						Dequeue from Running and enqueue to Ready
*						Dequeue from Ready, update CPU, and enqueue to Running
*						Set quantCount to 1
*						Go to while loop
*		If all queues have PCBs in them
*			If running PCB is on final CPU burst
*				If final CPU burst is complete
*					Dequeue from Running and send results to client
*					Increment clock
*					Decrement number of processes
*					Dequeue from Ready, update CPU, and enqueue to Running
*					Set quantCount to 1
*					If waiting PCB is has finished I/O processing
*						Dequeue from Waiting and enqueue to Ready
*						Print contents of queues
*						Go to while loop
*					Else
*						Keep processing I/O
*						Print contents of queues
*						Go to while loop
*				Else
*					If round robin time still remains
*						Keep running the process
*						Increment quantCount
*						If waiting PCB is has finished I/O processing
*							Dequeue from Waiting and enqueue to Ready
*							Go to while loop
*						Else
*							Keep processing I/O_RDONLY
*							Go to while loop
*					Else
*						Dequeue from Running and enqueue to Ready
*						Dequeue from Ready, update CPU, and enqueue to Running
*						Set quantCount to 1
*						If waiting PCB is has finished I/O processing
*							Dequeue from Waiting and enqueue to Ready
*							Go to while loop
*						Else
*							Keep processing I/O_RDONLY
*							Go to while loop
*			Else
*				Increment clock
*				If running PCB has finished processing current CPU burst
*					Dequeue from Running, update I/O, and enqueue to Waiting
*					Dequeue from Ready, update CPU, and enqueue to Running
*					Set quantCount to 1
*					If waiting PCB is has finished I/O processing
*						Dequeue from Waiting and enqueue to Ready
*						Print contents of queues
*						Go to while loop
*					Else
*						Keep processing I/O
*						Print contents of queues
*						Go to while loop
*				Else
*					If round robin time remains
*						Keep running the process
*						Increment quantCount
*						If waiting PCB is has finished I/O processing
*							Dequeue from Waiting and enqueue to Ready
*							Go to while loop
*						Else
*							Keep processing I/O_RDONLY
*							Go to while loop
*					Else
*						Dequeue from Running and enqueue to Ready
*						Dequeue from Ready, update CPU, and enqueue to Running
*						Set quantCount to 1
*						If waiting PCB is has finished I/O processing
*							Dequeue from Waiting and enqueue to Ready
*							Go to while loop
*						Else
*							Keep processing I/O_RDONLY
*							Go to while loop
*		If PCBs are in Running and Waiting but not in Ready
*			If running PCB is on final CPU burst
*				If final CPU burst has finished processing
*					Dequeue from Running and send results to client
*					Increment clock
*					Decrement number of processes
*					Dequeue from Ready, update CPU, and enqueue to Running
*					If waiting PCB has finished I/O processing
*						Dequeue from Waiting and enqueue to Running
*						Print contents of queues
*						Increment quantCounts
*						Go to while loop
*					Else
*						Set quantCount to 0 and increment idleCPUTime
*						Keep processing I/O
*						Print contents of queues
*						Go to while loop
*				Else
*					Increment clock
*					If round robin time still remains
*						Keep running the process
*						Increment quantCount
*						If waiting PCB has finished I/O processing
*							Dequeue from Waiting and enqueue to Ready
*							Go to while loop
*						Else
*							Keep processing I/O_RDONLY
*							Go to while loop
*					Else
*						If waiting PCB has finished I/O processing
*							Dequeue from Running and enqueue to Ready
*							Dequeue from Waiting, update CPU, and enqueue to Running
*							Print contents of queues
*							Set quantCount to 1
*							Go to while loop
*						Else
*							Keep processing current job's CPU burst
*							Set quantCount to 1
*							Keep processing I/O
*							Go to while loop
*			Else
*				Increment clock
*				If finished processing current CPU burst
*					Dequeue from Running and enqueue to Waiting
*					If waiting PCB is has finished I/O processing
*						Dequeue from Waiting and enqueue to Running
*						Print contents of queues
*						Set quantCount to 1
*						Go to while loop
*					Else
*						Set quantCount to 0 and increment idleCPUTime
*						Keep processing I/O
*						Print contents of queues
*						Go to while loop
*				Else 
*					If round robin time still remains
*						Keep running the process
*						Increment quantCount
*						If waiting PCB has finished I/O processing
*							Dequeue from Waiting and enqueue to Ready
*							Go to while loop
*						Else
*							Keep processing I/O
*							Go to while loop
*					Else
*						If waiting PCB has finished I/O processing
*							Dequeue from Running and enqueue to Ready
*							Dequeue from Waiting, update CPU, and enqueue to Running
*							Set quantCount to 1
*							Go to while loop
*						Else
*							Keep processing current job's CPU burst
*							Set quantCount to 1
*							Keep processing I/O
*							Go to while loop
*		If PCBs are only in Waiting
*			Increment clock
*			If waiting PCB has finished I/O processing
*				Dequeue from Waiting, update CPU, and enqueue to Running
*				Set quantCount to 1
*				Go to while loop
*			Else
*				Increment idleCPUTime
*				Keep processing I/O
*				Go to while loop
*	Display the CPU utilization for the session
*	Close read path from FIFO_to_Server
*	Unlink FIFO_to_Server
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
#include "queue.h"

// Begin Main Server Program
int main( int argc, int *argv[] ){

	// initialization
	float clock = 0;
	int quantCount = 0;
	int numclients = 0;
	float utilization = 0;
	int fda;		// for the FIFO_to_Server
	int numProcesses;
	float idleCPUTime = 0;
	int finish;
	ClientData clientdata;
	ServerData serverdata;
	
	// Prompt user for number of clients
	CLIENTS: printf("\nPlease enter how many clients will be processed (choose an integer between 3 and 10): ");
	scanf("%d", &numclients);
	if (numclients < 3 || numclients > 10) {
		goto CLIENTS;
	}
	
	printf("\n%d clients will be created.", numclients);
	printf("\nCPU bursts will be processed using round robin scheduling with a time quantum of 5 units.");
	printf("\nI/O bursts will be processed using a first come first serve scheduling scheme.");
	
	// create necessary arrays to store data for each client
	int fdb[numclients];
	
	// Create the common FIFO
	if ((mkfifo("FIFO_to_server",0666) < 0 && errno != EEXIST)) {
		perror("cant create FIFO_to_server");
		exit(-1);
	}
	
	// Open a read path from the common FIFO
	if( (fda = open("FIFO_to_server", O_RDONLY) ) < 0) {
		printf("cant open fifo to read");
	}
	
	// create and initialize the Ready queue
	Queue Ready;
	Ready.head = NULL;
	Ready.tail = NULL;
	Ready.sz = 0;
	
	// create and initialize the Running queue
	Queue Running;
	Running.head = NULL;
	Running.tail = NULL;
	Running.sz = 0;
	
	// create and initialize the Waiting queue
	Queue Waiting;
	Waiting.head = NULL;
	Waiting.tail = NULL;
	Waiting.sz = 0;

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
		
		// create a PCB object and store the client data in it and enqueue it to Ready
		// PCB will contain job number, private FIFO name, bursts array, and burst position
		Object object;
		object.jobnum = i;
		object.totalBursts = clientdata.totalBursts;
		strcpy(object.privateFIFO, clientdata.privateFIFO);
		memcpy(object.bursts, clientdata.bursts, sizeof(object.bursts));
		object.burstpos = 0;
		object.utilization = 0;
		enqueue(&Ready, object);
		
		// private FIFO was created on the client side
		// open a write path to the private FIFO
		if((fdb[i]=open(clientdata.privateFIFO, O_WRONLY))<0) {
			printf("\ncan't open fifo to write");
		}
	}
	
	/****************FUNCTIONS USED FOR PROCESSING*******************/
	
	// function to print node that was dequeued
	void printCompleted(Object object) {
		printf("\n********Completed Job*****************");
		printf("\nJob number = %d", object.jobnum);
		printf("\nPrivateFIFO = %s", object.privateFIFO);
		printf("\nutilization = %.2f percent", object.utilization);
		printf("\nCompletion time = %.0f", clock);
		printf("\n**************************************\n");
	}
	
	// function to finish PCB
	void FinishPCB(float clock, ServerData serverdata, int fdb[], Queue * const Running) {
		Object finishedpcb = dequeue(Running);
		serverdata.clock = clock;
		serverdata.utilization = (finishedpcb.totalBursts/clock)*100;
		finishedpcb.utilization = serverdata.utilization;
		printCompleted(finishedpcb);
		// write the result to the private FIFO
		write(fdb[finishedpcb.jobnum], &serverdata, sizeof(serverdata));
		// close the write path to the private FIFO
		close(fdb[finishedpcb.jobnum]);
	}
	
	// function to send from Ready to Running
	void ReadyToRunning(Queue * const Ready, Queue * const Running) {
		Object runcur = dequeue(Ready);
		int pos = runcur.burstpos;
		runcur.bursts[pos]--;
		enqueue(Running, runcur);
	}
	
	// function to send from Running to Waiting
	void RunningToWaiting(Queue * const Running, Queue * const Waiting) {
		Object gotowait = dequeue(Running);
		gotowait.burstpos++;
		if (isEmpty(Waiting)) {
			gotowait.bursts[gotowait.burstpos]--;
		}
		enqueue(Waiting, gotowait);
	}
	
	// function to send from Running to Ready
	void RunningToReady(Queue * const Running, Queue * const Ready) {
		Object gotoready = dequeue(Running);
		enqueue(Ready, gotoready);
	}
	
	// function to send from Waiting to Ready
	void WaitingToReady(Queue * const Waiting, Queue * const Ready) {
		Object waittoready = dequeue(Waiting);
		waittoready.burstpos++;
		enqueue(Ready, waittoready);
		if (!isEmpty(Waiting)) {
			Object waitcur = dequeue(Waiting);
			waitcur.bursts[waitcur.burstpos]--;
			enqueueToFront(Waiting, waitcur);
		}
	}
	
	// function to send from Waiting to Running
	void WaitingToRunning(Queue * const Waiting, Queue * const Running) {
		Object waittorun = dequeue(Waiting);
		waittorun.burstpos++;
		waittorun.bursts[waittorun.burstpos]--;
		enqueue(Running, waittorun);
		if (!isEmpty(Waiting)) {
			Object waitcur = dequeue(Waiting);
			waitcur.bursts[waitcur.burstpos]--;
			enqueueToFront(Waiting, waitcur);
		}
	}
	
	// function to keep running the current process
	void KeepRunning(Queue * const Running) {
		Object runcur = dequeue(Running);
		runcur.bursts[runcur.burstpos]--;
		enqueue(Running, runcur);
	}
	
	// function to keep running the I/O burst
	void KeepIO(Queue * const Waiting) {
		Object waitcur = dequeue(Waiting);
		waitcur.bursts[waitcur.burstpos]--;
		enqueueToFront(Waiting, waitcur);
	}
	
	/****************PROCESSING BURSTS****************/
	printf("\n*******The Ready queue at time 0*******");
	traverseQueue(&Ready, visitPCB);
	
	numProcesses = numclients;
	PROCESSING: while (numProcesses>0) {
		// initial case, time 0, all PCBs are in Ready
		if (!isEmpty(&Ready) && isEmpty(&Running) && isEmpty(&Waiting)) {
			ReadyToRunning(&Ready, &Running);
			quantCount = 1;
			clock++;
			goto PROCESSING;
		}
		
		// final case, only one PCB left and in Running
		if (isEmpty(&Ready) && !isEmpty(&Running) && isEmpty(&Waiting)) {
			Object runcur = first(&Running);
			int pos = runcur.burstpos;
			// if on the final CPU burst
			if (runcur.bursts[pos + 1] < 0) {
				// if the burst is complete
				if (runcur.bursts[pos] == 0) {
					FinishPCB(clock, serverdata, fdb, &Running);
					numProcesses--;
					clock++;
					goto PROCESSING;
				}
				// if there is more to process in the final burst
				else {
					KeepRunning(&Running);
					quantCount++;
					clock++;
					goto PROCESSING;
				}
			}
			// if there are more CPU bursts left after the current one
			else {
				// if this CPU burst is finished
				if (runcur.bursts[pos] == 0) {
					RunningToWaiting(&Running, &Waiting);
					clock++;
					quantCount = 0;
					printf("\nAt time %.0f:", clock);
					printf("\n*******The contents of the Ready queue*******");
					traverseQueue(&Ready, visitPCB);
					printf("\n*******The contents of the Waiting queue*******");
					traverseQueue(&Waiting, visitPCB);
					goto PROCESSING;
				}
				// if this CPU burst isn't finished
				else {
					runcur.bursts[pos]--;
					quantCount++;
					clock++;
					goto PROCESSING;
				}
			}
		}
		
		// time 1 and other instances, where nothing is in Waiting
		if (!isEmpty(&Ready) && !isEmpty(&Running) && isEmpty(&Waiting)) {
			Object runcur = first(&Running);
			int pos = runcur.burstpos;
			// if on final CPU burst
			if (runcur.bursts[pos + 1] < 0) {
				// if finished all processing
				if (runcur.bursts[pos] == 0) {
					FinishPCB(clock, serverdata, fdb, &Running);
					clock++;
					numProcesses--;
					ReadyToRunning(&Ready, &Running);
					quantCount = 1;
					printf("\nAt time %.0f:", clock);
					printf("\n*******The contents of the Ready queue*******");
					traverseQueue(&Ready, visitPCB);
					printf("\n*******The contents of the Waiting queue********");
					traverseQueue(&Waiting, visitPCB);
					goto PROCESSING;
				}
				// if final CPU burst still needs to be processed
				else {
					// if round robin time isn't up
					if (quantCount < 5) {
						KeepRunning(&Running);
						quantCount++;
						clock++;
						goto PROCESSING;
					}
					// if round robin time is up
					else {
						RunningToReady(&Running, &Ready);
						ReadyToRunning(&Ready, &Running);
						quantCount = 1;
						clock++;
						goto PROCESSING;
					}
				}
			}
			// if not on final CPU burst
			else {
				clock++;
				// if this CPU burst has finished processing
				if (runcur.bursts[pos] == 0) {
					RunningToWaiting(&Running, &Waiting);
					ReadyToRunning(&Ready, &Running);
					quantCount = 1;
					printf("\nAt time %.0f:", clock);
					printf("\n*******The contents of the Ready queue*******");
					traverseQueue(&Ready, visitPCB);
					printf("\n*******The contents of the Waiting queue*******");
					traverseQueue(&Waiting, visitPCB);
					goto PROCESSING;
				}
				// if the CPU burst still needs processing
				else {
					// if round robin time isn't up
					if (quantCount < 5) {
						KeepRunning(&Running);
						quantCount++;
						goto PROCESSING;
					}
					// if round robin time is up
					else {
						RunningToReady(&Running, &Ready);
						ReadyToRunning(&Ready, &Running);
						quantCount = 1;
						goto PROCESSING;
					}
				}
			}
		}
		
		// if all queues are busy
		if (!isEmpty(&Ready) && !isEmpty(&Running) && !isEmpty(&Waiting)) {
			Object runcur = first(&Running);
			int rpos = runcur.burstpos;
			Object waitcur = first(&Waiting);
			int wpos = waitcur.burstpos;
			// if runcur is on final CPU burst
			if (runcur.bursts[rpos + 1] < 0) {
				// if it has finished final CPU burst
				if (runcur.bursts[rpos] == 0) {
					FinishPCB(clock, serverdata, fdb, &Running);
					clock++;
					numProcesses--;
					ReadyToRunning(&Ready, &Running);
					quantCount = 1;
					printf("\nAt time %.0f:", clock);
					// if waitcur has finished I/O processing
					if (waitcur.bursts[wpos] == 0) {
						WaitingToReady(&Waiting, &Ready);
						printf("\n*******The contents of the Ready queue*******");
						traverseQueue(&Ready, visitPCB);
						printf("\n*******The contents of the Waiting queue*******");
						traverseQueue(&Waiting, visitPCB);
						goto PROCESSING;
					}
					// if waitcur is still processing I/O
					else {
						KeepIO(&Waiting);
						printf("\n*******The contents of the Ready queue*******");
						traverseQueue(&Ready, visitPCB);
						printf("\n*******The contents of the Waiting queue*******");
						traverseQueue(&Waiting, visitPCB);
						goto PROCESSING;
					}
				}
				// if it hasn't finished its final CPU burst
				else {
					clock++;
					// if still has round robin time remaining
					if (quantCount < 5) {
						KeepRunning(&Running);
						quantCount++;
						// if waitcur has finished I/O processing
						if (waitcur.bursts[wpos] == 0) {
							WaitingToReady(&Waiting, &Ready);
							goto PROCESSING;
						}
						// if waitcur has not finished I/O
						else {
							KeepIO(&Waiting);
							goto PROCESSING;
						}
					}
					// if round robin time is up
					else {
						RunningToReady(&Running, &Ready);
						ReadyToRunning(&Ready, &Running);
						quantCount = 1;
						// if waitcur has finished I/O
						if (waitcur.bursts[wpos] == 0) {
							WaitingToReady(&Waiting, &Ready);
							goto PROCESSING;
						}
						// if waitcur hasn't finished I/O
						else {
							KeepIO(&Waiting);
							goto PROCESSING;
						}
					}
				}
			}
			// if runcur has more CPU bursts to process after this one
			else {
				clock++;
				// if runcur finished current CPU burst
				if (runcur.bursts[rpos] == 0) {
					RunningToWaiting(&Running, &Waiting);
					ReadyToRunning(&Ready, &Running);
					printf("\nAt time %.0f:", clock);
					quantCount = 1;
					// if waitcur has finished I/O
					if (waitcur.bursts[wpos] == 0) {
						WaitingToReady(&Waiting, &Ready);
						printf("\n*******The contents of the Ready queue*******");
						traverseQueue(&Ready, visitPCB);
						printf("\n*******The contents of the Waiting queue*******");
						traverseQueue(&Waiting, visitPCB);
						goto PROCESSING;
					}
					// if waitcur not finished I/O
					else {
						KeepIO(&Waiting);
						printf("\n*******The contents of the Ready queue*******");
						traverseQueue(&Ready, visitPCB);
						printf("\n*******The contents of the Waiting queue*******");
						traverseQueue(&Waiting, visitPCB);
						goto PROCESSING;
					}
				}
				// if runcur has not finished CPU burst
				else {
					// if round robin time is still remaining
					if (quantCount < 5) {
						KeepRunning(&Running);
						quantCount++;
						// if waitcur finished I/O
						if (waitcur.bursts[wpos] == 0) {
							WaitingToReady(&Waiting, &Ready);
							goto PROCESSING;
						}
						// if waitcur is still doing I/O
						else {
							KeepIO(&Waiting);
							goto PROCESSING;
						}
					}
					// if round robin time is up
					else {
						RunningToReady(&Running, &Ready);
						ReadyToRunning(&Ready, &Running);
						quantCount = 1;
						//	if waitcur finished I/O
						if (waitcur.bursts[wpos] == 0) {
							WaitingToReady(&Waiting, &Ready);
							goto PROCESSING;
						}
						// if waitcur is still doing I/O
						else {
							KeepIO(&Waiting);
							goto PROCESSING;
						}
					}
				}
			}
		}
		
		// if there is nothing in Ready
		if (isEmpty(&Ready) && !isEmpty(&Running) && !isEmpty(&Waiting)) {
			Object runcur = first(&Running);
			int rpos = runcur.burstpos;
			Object waitcur = first(&Waiting);
			int wpos = waitcur.burstpos;
			// if runcur is on final CPU burst
			if (runcur.bursts[rpos + 1] < 0) {
				// if all of its processing is finished
				if (runcur.bursts[rpos] == 0) {
					FinishPCB(clock, serverdata, fdb, &Running);
					clock++;
					numProcesses--;
					// if waitcur finished with I/O
					if (waitcur.bursts[wpos] == 0) {
						WaitingToRunning(&Waiting, &Running);
						printf("\nAt time %.0f:", clock);
						printf("\n*******The contents of the Ready queue*******");
						traverseQueue(&Ready, visitPCB);
						printf("\n*******The contents of the Waiting queue*******");
						traverseQueue(&Waiting, visitPCB);
						quantCount = 1;
						goto PROCESSING;	
					}
					// if not finished I/O
					else {
						quantCount = 0;
						idleCPUTime++;
						KeepIO(&Waiting);
						printf("\nAt time %.0f:", clock);
						printf("\n*******The contents of the Ready queue*******");
						traverseQueue(&Ready, visitPCB);
						printf("\n*******The contents of the Waiting queue*******");
						traverseQueue(&Waiting, visitPCB);
						goto PROCESSING;
					}
				}
				// if it still has CPU processing
				else {
					clock++;
					// if round robin time is still remaining
					if (quantCount < 5) {
						KeepRunning(&Running);
						quantCount++;
						// if I/O finished
						if (waitcur.bursts[wpos] == 0) {
							WaitingToReady(&Waiting, &Ready);
							goto PROCESSING;
						}
						// if I/O remaining
						else {
							KeepIO(&Waiting);
							goto PROCESSING;
						}
					}
					// if round robin time is up
					else {
						// if I/O finished
						if (waitcur.bursts[wpos] == 0) {
							RunningToReady(&Running, &Ready);
							WaitingToRunning(&Waiting, &Running);
							printf("\nAt time %.0f:", clock);
							printf("\n*******The contents of the Ready queue*******");
							traverseQueue(&Ready, visitPCB);
							printf("\n*******The contents of the Waiting queue*******");
							traverseQueue(&Waiting, visitPCB);
							quantCount = 1;
							goto PROCESSING;
						}
						// if I/O remaining
						else {
							KeepRunning(&Running);
							quantCount = 1;
							KeepIO(&Waiting);
							goto PROCESSING;
						}
					}
				}
			}
			// if not on final CPU burst
			else {
				clock++;
				// if finished with current CPU burst
				if (runcur.bursts[rpos] == 0) {
					printf("\nAt time %.0f:", clock);
					RunningToWaiting(&Running, &Waiting);
					// if I/O finished
					if (waitcur.bursts[wpos] == 0) {
						WaitingToRunning(&Waiting, &Running);
						printf("\n*******The contents of the Ready queue*******");
						traverseQueue(&Ready, visitPCB);
						printf("\n*******The contents of the Waiting queue*******");
						traverseQueue(&Waiting, visitPCB);
						quantCount = 1;
						goto PROCESSING;
					}
					// if I/O remaining
					else {
						quantCount = 0;
						idleCPUTime++;
						KeepIO(&Waiting);
						printf("\n*******The contents of the Ready queue*******");
						traverseQueue(&Ready, visitPCB);
						printf("\n*******The contents of the Waiting queue*******");
						traverseQueue(&Waiting, visitPCB);
						goto PROCESSING;
					}
				}
				// if CPU burst still remaining
				else {
					// if round robin time still remaining
					if (quantCount < 5) {
						KeepRunning(&Running);
						quantCount++;
						// if I/O finished
						if (waitcur.bursts[wpos] == 0) {
							WaitingToReady(&Waiting, &Ready);
							goto PROCESSING;
						}
						// if I/O remaining
						else {
							KeepIO(&Waiting);
							goto PROCESSING;
						}
					}
					// if round robin time is finished
					else {
						// if I/O finished
						if (waitcur.bursts[wpos] == 0) {
							RunningToReady(&Running, &Ready);
							WaitingToRunning(&Waiting, &Running);
							quantCount = 1;
							goto PROCESSING;
						}
						// if I/O remaining
						else {
							KeepRunning(&Running);
							quantCount = 1;
							KeepIO(&Waiting);
							goto PROCESSING;
						}
					}
				}
			}
		}
		
		// if PCBs are only in Waiting
		if (isEmpty(&Ready) && isEmpty(&Running) && !isEmpty(&Waiting)) {
			clock++;
			Object waitcur = first(&Waiting);
			int pos = waitcur.burstpos;
			// if finished I/O processing
			if (waitcur.bursts[pos] == 0) {
				WaitingToRunning(&Waiting, &Running);
				quantCount = 1;
				goto PROCESSING;
			}
			// if not finished I/O
			else {
				idleCPUTime++;
				KeepIO(&Waiting);
				goto PROCESSING;
			}
		}
	}
	
	// Display the CPU utilzation
	float activeCPUTime = clock - idleCPUTime;
	utilization = (activeCPUTime/clock)*100;
	printf("\n****************************************************");
	printf("\nTotal CPU Utilization was %.2f percent.", utilization);
	printf("\n****************************************************\n");
	
	// close the read path from the common FIFO
	close(fda);
	printf("\nClosed read path from common FIFO.");

	// unlink the common FIFO
	unlink("FIFO_to_server");
	printf("\nUnlinked the common FIFO.\n");
		
}
		
	
	
	






