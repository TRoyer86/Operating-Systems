typedef struct {
	char privateFIFO[14];
	int burst;
	int arrivalTime;
} ClientData;

typedef struct {
	int clock;
	int turnaround;
} ServerData;


