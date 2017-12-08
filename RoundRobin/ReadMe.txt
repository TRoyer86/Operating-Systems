What is this?
-------------------------------------------
RRServer and RRClient are a set of programs that emulate a round robin process scheduling scheme. RRServer
is run and accepts a feed of input from multiple clients running RRClient. RRServer processes the jobs and
their burst requests and sends back to each client the time its process finished as well as its turnaround
time. RRServer also displays the average turnaround time for all of the jobs it processed.

How to use it
-------------------------------------------
1. Make sure that rrServer.c, rrClient.c, and rrcommon.h are in the same directory.
2. Open up at least four terminals, one to run RRServer and the others to run RRClient.
3. Type the following command to compile RRServer: gcc rrServer.c -o RRServer.
4. Type the following command to compile RRClient: gcc rrClient.c -o RRClient.
5. In the first terminal, type the following command to run RRServer: ./RRServer.
6. In RRServer, input the number of clients and the time quant value.
7. In the other terminals, one at a time, type the following command to run RRClient: ./RRClient.
8. For each RRClient, input the arrival time of the job and its burst request. 
	(IMPORTANT NOTE: Arrival time matters. Each RRClient that is run must have an arrival time that 
	is larger than the previous client's arrival time. For example, if client 1 had an arrival time 
	of 0, client 2 would have an arrival time of 1 or more, etc.) 
9. The server receives the data from each client, processes each job, and sends the response back to each
   client. RRClient prints the time the job was completed and the turnaround time. RRServer prints the
   average turnaround time for all of the jobs it processed.
