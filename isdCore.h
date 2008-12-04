# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <GL/glut.h>

#define DEBUG		0

#define WINDOW_HEIGHT	400
#define WINDOW_WIDTH	640
#define POSITION_X	5
#define POSITION_Y	5
#define WINDOW_TITLE	"isdGLR!"

#define MSG_USAGE	"USAGE: ./isdGLR <data-filename>\nLOADING: Default values\n"
#define MSG_ERROR_IO	"\nWARNING: Cannot read from file\nLOADING: Default values\n"
#define ERRORISDGL	-1
#define SUCCESSISDGL	1

typedef struct lorenz {
	// Values to be plotted
	double x,y,z;
	// Lorenz Parameters
	double sigma,ro,beta;
	// Time
	double t;
	// Time variation
	double dt;
	// Max allowed iterations
	int maxiters;
	// Color scheme for trajectory
	int scheme;
	// Style (1 for points, 2 for dashed, other for line)
	int style;
} Lorenz;


typedef struct point {
	double x,y,z;
	float r,g,b;
	float opacity;
} Point;

int readLorenzDataFile(char * filePath, Lorenz * lorenzData);
void defaultLorenzValues(Lorenz * lorenzData);
void doLorenzStep(Lorenz * lorenzData);
void initializeNonParameters(Lorenz * lorenzData);
