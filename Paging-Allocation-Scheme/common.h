// used to send data from client to server
typedef struct {
	char privateFIFO[14];
	int memrequest;
	char jobname[20];
} JobData;

// used to send result from server to client
typedef struct {
	int frag;
	int startframe;
	int endframe;
	char msg[100];
} ResultData;


