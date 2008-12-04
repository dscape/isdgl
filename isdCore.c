#include "isdCore.h"

int readLorenzDataFile(
	char * filePath,
	Lorenz * lorenzData
)
{
	FILE *f;
 	initializeNonParameters(lorenzData);

	if((f = fopen(filePath,"r+"))==NULL)
		return ERRORISDGL;

	fscanf(f,"ro: %lf\nsigma: %lf\nbeta: %lf\ndt: %lf\nmaxiters: %d\ncolorscheme: %d\nstyle: %d",&lorenzData->ro,&lorenzData->sigma,
									 &lorenzData->beta,&lorenzData->dt,&lorenzData->maxiters,
									 &lorenzData->scheme, &lorenzData->style);

	if(DEBUG)
		printf("ro: %lf\nsigma: %lf\nbeta: %lf\ndt: %lf\nmaxiters: %d\ncolorscheme: %d\nstyle: %d\n",lorenzData->ro,lorenzData->sigma,
									 lorenzData->beta,lorenzData->dt,lorenzData->maxiters,
									 lorenzData->scheme, lorenzData->style);
	fclose(f);

	return SUCCESSISDGL;
}

void defaultLorenzValues(
	Lorenz * lorenzData
)
{
	initializeNonParameters(lorenzData);
	sscanf("28 10 2.667","%lf %lf %lf",&lorenzData->ro, &lorenzData->sigma, &lorenzData->beta);
	sscanf("0.02","%lf",&lorenzData->dt);
	lorenzData->maxiters = 30000;
	lorenzData->scheme = 1;
	lorenzData->style  = 1;
}

void initializeNonParameters(
	Lorenz * lorenzData
)
{
	lorenzData->x=3.f;
	lorenzData->y=15.f;
	lorenzData->z=1.f;
	lorenzData->t=0.f;
}

void doLorenzStep(
	Lorenz * lorenzData
)
{
	double _x=0.f,_y=0.f,_z=0.f;

	_x = lorenzData->x + lorenzData->dt*lorenzData->sigma*(lorenzData->y-lorenzData->x);
	_y = lorenzData->y + lorenzData->dt*(lorenzData->x*(lorenzData->ro-lorenzData->z)-lorenzData->y);
	_z = lorenzData->z + lorenzData->dt*(lorenzData->x*lorenzData->y-lorenzData->beta*lorenzData->z);
	lorenzData->x = _x;
	lorenzData->y = _y;
	lorenzData->z = _z;

	if(DEBUG)
		printf("x: %lf y: %lf z: %lf\n",_x,_y,_z);
}
