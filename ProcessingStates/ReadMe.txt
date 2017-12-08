What is this?
-------------------------------------------
ProcStatesServer and ProcStatesClient are a set of programs that emulate a round robin process scheduling scheme for CPU
bursts and a first come first server scheduling scheme for I/O bursts. ProcStatesServer is run and accepts a feed of input from multiple 
clients running ProcStatesClient. RRServer processes the jobs and their burst requests and sends back to each client the time its process 
finished as well as the percentage its job was spent in CPU and I/O processing. RRServer also displays the CPU utilization time
for each session.

How to use it
-------------------------------------------
1. Make sure that ProcStatesClient.c, ProcStatesServer.c, queue.c, queue.h, and pscommon.h are in the same directory.
2. Open up at least four terminals, one to run ProcStatesServer and the others to run ProcStatesClient.
3. Type the following command to compile ProcStatesServer: gcc ProcStatesServer.c queue.c -o ProcStatesServer.
4. Type the following command to compile ProcStatesClient: gcc ProcStatesClient.c -o ProcStatesClient.
5. In the first terminal, type the following command to run ProcStatesServer: ./ProcStatesServer.
6. In ProcStatesServer, input the number of clients.
7. In the other terminals, one at a time, type the following command to run ProcStatesClient: ./RRClient.
8. For each ProcStatesClient, input the number of bursts requested and then the burst requests. 
9. The server receives the data from each client, processes each job, and sends the response back to each
   client. ProcStatesClient prints the time the job was completed and the percentage of time it spent processing its bursts. 
   RRServer prints the CPU utilization for the session.
