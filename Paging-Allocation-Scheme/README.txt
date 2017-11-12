What is this?
-------------------------------------------
Run together, pagingClient and pagingServer demonstrate a paging memory allocation scheme. pagingServer 
creates a named pipe through which each pagingClient sends a job name, job request size, and the name of
a private named pipe. pagingServer reads the request and assigns the job to memory and sends the results
back to pagingClient. At the end, pagingServer displays a memory map of the requests.

How to use it
-------------------------------------------
1. Make sure that pagingServer.c, pagingClient.c, and common.h are in the same directory.
2. Open up two terminals, one to run pagingClient and the other to run pagingServer.
3. Type the following command to compile pagingServer: gcc pagingServer.c -o pagingServer.
4. Type the following command to compile pagingClient: gcc pagingClient.c -o pagingClient.
5. In the first terminal, type the following command to run pagingServer: ./pagingServer.
6. In the second terminal, type the following command to run pagingClient: ./pagingClient.
7. In pagingServer, input the number of clients and the number of frames in memory.
8. In pagingClient, input the job name and the memory request size. 
9. The server receives the data, calculates the memory allocation, and sends the response back to the
   client. pagingClient prints the result and exits. pagingServer is still running and waiting for
   more data from pagingClient.
10. Repeat steps 6 through 9 until pagingServer displays the memory map and exits. Once it has finished,
	pagingClient will have finished as well.