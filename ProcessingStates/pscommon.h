// create the client struct and instantiate it
typedef struct {
char privateFIFO[14];
int bursts[10];
float totalBursts;
} ClientData;

// create the server struct and instantiate it
typedef struct {
float clock;
float utilization;
} ServerData;


