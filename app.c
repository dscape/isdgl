#include "isdGL.h"

// OpenGL Window
int windowID;
int width  = WINDOW_WIDTH;
int height = WINDOW_HEIGHT;
int step   = 0;


// OpenGL 3D
float angle = 25, angley=0.0, deltaAngle = 0.0, deltaAngleY = 0.0, ratio;
float x = 85.0f, y = 20.0f, z = 90.0f;
float lx = -0.5f, ly = 0.0f, lz = 0.0f;
GLint pointDisplayList;
int deltaMove = 0;

// Starts/stop model action
int started=0;

float opacity=1.0f;
int opacityUp=0;
int pixopflag=0;

// Data model
Lorenz lorenzData;

// List of calculated points
Point * points;

int maxiters,scheme,style;

int main(
	int argc,
	char **argv
)
{
	int f=1;

	if(argc != 2) {
		printf(MSG_USAGE);
		f=0;
	} else if(f && readLorenzDataFile(argv[1],&lorenzData) == ERRORISDGL) {
		if(DEBUG)
			printf(MSG_ERROR_IO);
		showErrorGL(MSG_ERROR_IO);
		f=0;
	}

	if(!f)
		defaultLorenzValues(&lorenzData);

	maxiters = lorenzData.maxiters;
	scheme   = lorenzData.scheme;
	style    = lorenzData.style;
	points = malloc(sizeof(Point)*maxiters);

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width,height);
	glutInitWindowPosition(POSITION_X,POSITION_Y);
	windowID = glutCreateWindow(WINDOW_TITLE);
	glutGameModeString("1280x800:32@60");
	glutEnterGameMode();
	initializeGL();

	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(&keyPressedGL);
	glutSpecialFunc(&specialKeyPressedGL);
	glutSpecialUpFunc(releaseKeyGL);

	glutDisplayFunc(drawGL);
	glutReshapeFunc(reshapeGL);
	glutTimerFunc(20, timerGL, 1);
	glutMainLoop();
	return EXIT_SUCCESS;
}

void initializeGL() {
	GLfloat lightENV[4]={0.2,0.2,0.2,1.0}; 
	GLfloat lightDIF[4]={0.7,0.7,0.7,1.0};
	GLfloat lightESP[4]={1.0, 1.0, 1.0, 1.0}; 
	GLfloat lightPOS[4]={85.0, 20.0, 90.0, 1.0};

	GLfloat spectrum[4]={1.0,1.0,1.0,1.0}; 
	GLint spectrumMAT = 60;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	glShadeModel(GL_SMOOTH);

	glMaterialfv(GL_FRONT,GL_SPECULAR, spectrum);
	glMateriali(GL_FRONT,GL_SHININESS,spectrumMAT);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lightENV);

	glLightfv(GL_LIGHT0, GL_AMBIENT, lightENV); 
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDIF );
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightESP );
	glLightfv(GL_LIGHT0, GL_POSITION, lightPOS );

	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);  
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);
	pointDisplayList = createDL();
}

void drawGL() {
	int i;
	if (deltaMove)
		moveGL(deltaMove);

	if (deltaAngle) {
		angle += deltaAngle;
		orientGL();
	}

	if(deltaAngleY){
		angley += deltaAngleY;
		orientGL();
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	 glEnable(GL_BLEND);
	 glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Draw Yellow REF
	glColor4f(1.0f,0.0f,0.0f,opacity);
	glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.0f, 0.0f);
	glEnd();
	glColor4f(0.0f,1.0f,0.0f,opacity);
	glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);
	glEnd();
	glColor4f(0.0f,0.0f,1.0f,opacity);
	glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 1.0f);
	glEnd();

	// Draw Lines
	switch(style) {
		case 1:	// Points
			for(i=0;i<=step;++i)
			{
				if(pixopflag) {
					glColor4f(points[i].r,points[i].g,points[i].b,points[i].opacity);
				}
				else
					glColor4f(points[i].r,points[i].g,points[i].b,opacity);

				glBegin(GL_POINTS);
					glVertex3f(points[i].x, points[i].y, points[i].z);
				glEnd();		
			}
			break;
		case 2:	// Dashed
			for(i=2;i<=step-2;i+=2)
			{
				if(pixopflag) {
					glColor4f(points[i-2].r,points[i-2].g,points[i-2].b,points[i-2].opacity);
				} else
					glColor4f(points[i-2].r,points[i-2].g,points[i-2].b,opacity);
				glBegin(GL_LINES);
					glVertex3f(points[i-2].x, points[i-2].y, points[i-2].z);
					glVertex3f(points[i-1].x, points[i-1].y, points[i-1].z);
				glEnd();		
			}
			break;
		default: // Line
			for(i=2;i<=step-2;i++)
			{
				if(pixopflag) {
					glColor4f(points[i-2].r,points[i-2].g,points[i-2].b,points[i-2].opacity);
				} else
					glColor4f(points[i-2].r,points[i-2].g,points[i-2].b,opacity);
				glBegin(GL_LINES);
					glVertex3f(points[i-2].x, points[i-2].y, points[i-2].z);
					glVertex3f(points[i-1].x, points[i-1].y, points[i-1].z);
				glEnd();		
			}
			break;
	}


	// Draw point
	glPushMatrix();
	glTranslatef(lorenzData.x,lorenzData.y,lorenzData.z);
	glCallList(pointDisplayList);
	glPopMatrix();

	glutSwapBuffers();
}

void reshapeGL(
	GLsizei w,
	GLsizei h
)
{
	ratio = 1.0f * w / h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glViewport(0, 0, w, h);

	gluPerspective(45,ratio,1,1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(x, y, z, 
		      x + lx,y + ly,z + lz,
			  0.0f,1.0f,0.0f);
}

void timerGL(
	int t
)
{
	if(started && maxiters > step) {
		doLorenzStep(&lorenzData);
		points[step].x = lorenzData.x;
		points[step].y = lorenzData.y;
		points[step].z = lorenzData.z;
		switch(scheme) {
			case 1:
				points[step].r = (float) step/maxiters;
				points[step].g = (float) step/maxiters;
				points[step].b = (float) (maxiters-step)/maxiters;
				break;
			case 2:
				points[step].r = (float)step/maxiters;
				points[step].g = (float)step/maxiters;
				points[step].b = 1.0f;
				break;
			case 3:
				points[step].r = (float)step/maxiters;
				points[step].g = 1.0f;
				points[step].b = (float)step/maxiters;
				break;
			case 4:
				points[step].r = 1.0f;
				points[step].g = (float)step/maxiters;
				points[step].b = (float)step/maxiters;
				break;
			case 5:
				points[step].r = (float) step/maxiters;
				points[step].g = (float) step/maxiters;
				points[step].b = (float) step/maxiters;
				break;
			case 6:
				points[step].r = 1.0f;
				points[step].g = (float)rand()/RAND_MAX;
				points[step].b = 0.0f;
				break;
			case 7:
				points[step].r = (float) step/maxiters;
				points[step].g = (float) (maxiters-step)/maxiters;
				points[step].b = 0.0f;
				break;
			case 8:
				points[step].r = (float) step/maxiters;
				points[step].g = 0.0f;
				points[step].b = (float) (maxiters-step)/maxiters;
				break;
			case 9:
				points[step].r = 1.0f;
				points[step].g = (float) (points[step].y-points[step].x)/(sqrt(((points[step].y-points[step].x)*
						(points[step].y-points[step].x))+((points[step].z-points[step].x)*
						(points[step].z-points[step].x)))); // sin
				points[step].b = 0.0f;
				break;
			case 10:
				points[step].r = 1.0f;
				points[step].g = 1.0f;
				points[step].b = (float) (points[step].y-points[step].x)/(sqrt(((points[step].y-points[step].x)*
						(points[step].y-points[step].x))+((points[step].z-points[step].x)*
						(points[step].z-points[step].x))));
				break;
			case 11:
				points[step].r = 0.0f;
				points[step].g = 0.0f;
				points[step].b = (float) (points[step].y-points[step].x)/(sqrt(((points[step].y-points[step].x)*
						(points[step].y-points[step].x))+((points[step].z-points[step].x)*
						(points[step].z-points[step].x))));
				break;
			case 12:
				if(step<20){
					points[step].r = 0.0f;
					points[step].g = 0.0f;
					points[step].b = 0.0f;
				} else {
					points[step-20].r = (float) (points[step-20].y-points[step-20].x)/(sqrt(((points[step-20].y-points[step-20].x)*
						(points[step-20].y-points[step-20].x))+((points[step-20].z-points[step-20].x)*
						(points[step-20].z-points[step-20].x))));
					points[step-10].g = (float) (points[step-10].y-points[step-10].x)/(sqrt(((points[step-10].y-points[step-10].x)*
						(points[step-10].y-points[step-10].x))+((points[step-10].z-points[step-10].x)*
						(points[step-10].z-points[step-10].x))));
					points[step].b = (float) (points[step].y-points[step].x)/(sqrt(((points[step].y-points[step].x)*
						(points[step].y-points[step].x))+((points[step].z-points[step].x)*
						(points[step].z-points[step].x))));
				}
				break;
			case 13:
				points[step].r = 0.0f;
				points[step].g = 1.0f;
				points[step].b = (float) (points[step].y-points[step].x)/(sqrt(((points[step].y-points[step].x)*
						(points[step].y-points[step].x))+((points[step].z-points[step].x)*
						(points[step].z-points[step].x))));
				break;
			case 14:
				if(step<20){
					points[step].r = 0.0f;
					points[step].g = 0.0f;
					points[step].b = 0.0f;
				} else {
					points[step-20].r = (float) (points[step-20].y-points[step-20].x)/(sqrt(((points[step-20].y-points[step-20].x)*
						(points[step-20].y-points[step-20].x))+((points[step-20].z-points[step-20].x)*
						(points[step-20].z-points[step-20].x))));
					points[step-10].g = 1.0f;
					points[step].b = (float) (points[step].y-points[step].x)/(sqrt(((points[step].y-points[step].x)*
						(points[step].y-points[step].x))+((points[step].z-points[step].x)*
						(points[step].z-points[step].x))));
				}
				break;
			case 15:
				if(step<20){
					points[step].r = 0.0f;
					points[step].g = 0.0f;
					points[step].b = 0.0f;
				} else {
					points[step-20].r = 1.0f;
					points[step-10].g = (float) (points[step-10].y-points[step-10].x)/(sqrt(((points[step-10].y-points[step-10].x)*
						(points[step-10].y-points[step-10].x))+((points[step-10].z-points[step-10].x)*
						(points[step-10].z-points[step-10].x))));
					points[step].b = (float) (points[step].y-points[step].x)/(sqrt(((points[step].y-points[step].x)*
						(points[step].y-points[step].x))+((points[step].z-points[step].x)*
						(points[step].z-points[step].x))));
				}
				break;
			case 16:
				if(step<20){
					points[step].r = 0.0f;
					points[step].g = 0.0f;
					points[step].b = 0.0f;
				} else {
					points[step].r = (float) (points[step-20].y-points[step-20].x)/(sqrt(((points[step-20].y-points[step-20].x)*
						(points[step-20].y-points[step-20].x))+((points[step-20].z-points[step-20].x)*
						(points[step-20].z-points[step-20].x))));
					points[step].g = (float) (points[step-10].y-points[step-10].x)/(sqrt(((points[step-10].y-points[step-10].x)*
						(points[step-10].y-points[step-10].x))+((points[step-10].z-points[step-10].x)*
						(points[step-10].z-points[step-10].x))));
					points[step].b = (float) (points[step].y-points[step].x)/(sqrt(((points[step].y-points[step].x)*
						(points[step].y-points[step].x))+((points[step].z-points[step].x)*
						(points[step].z-points[step].x))));
				}
				break;
			case 17:
				if(step<20){
					points[step].r = 0.0f;
					points[step].g = 0.0f;
					points[step].b = 0.0f;
				} else {
					points[step].r = 1.0f;
					points[step].g = (float) (points[step-10].y-points[step-10].x)/(sqrt(((points[step-10].y-points[step-10].x)*
						(points[step-10].y-points[step-10].x))+((points[step-10].z-points[step-10].x)*
						(points[step-10].z-points[step-10].x))));
					points[step].b = (float) (points[step].y-points[step].x)/(sqrt(((points[step].y-points[step].x)*
						(points[step].y-points[step].x))+((points[step].z-points[step].x)*
						(points[step].z-points[step].x))));
				}
				break;
			case 18:
				if(step<20){
					points[step].r = 0.0f;
					points[step].g = 0.0f;
					points[step].b = 0.0f;
				} else {
					points[step].g = 1.0f;
					points[step].r = (float) (points[step-10].y-points[step-10].x)/(sqrt(((points[step-10].y-points[step-10].x)*
						(points[step-10].y-points[step-10].x))+((points[step-10].z-points[step-10].x)*
						(points[step-10].z-points[step-10].x))));
					points[step].b = (float) (points[step].y-points[step].x)/(sqrt(((points[step].y-points[step].x)*
						(points[step].y-points[step].x))+((points[step].z-points[step].x)*
						(points[step].z-points[step].x))));
				}
				break;
			case 19:
				if(step<20){
					points[step].r = 0.0f;
					points[step].g = 0.0f;
					points[step].b = 0.0f;
				} else {
					points[step].b = 1.0f;
					points[step].g = (float) (points[step-10].y-points[step-10].x)/(sqrt(((points[step-10].y-points[step-10].x)*
						(points[step-10].y-points[step-10].x))+((points[step-10].z-points[step-10].x)*
						(points[step-10].z-points[step-10].x))));
					points[step].r = (float) (points[step].y-points[step].x)/(sqrt(((points[step].y-points[step].x)*
						(points[step].y-points[step].x))+((points[step].z-points[step].x)*
						(points[step].z-points[step].x))));
				}
				break;
			default:
				points[step].r = (float)rand()/RAND_MAX;
				points[step].g = (float)rand()/RAND_MAX;
				points[step].b = (float)rand()/RAND_MAX;
				break;
		}

		points[step].opacity = (float) (points[step].y-points[step].x)/(sqrt(((points[step].z-points[step].x)*
						(points[step].z-points[step].x))+((points[step].y-points[step].x)*
						(points[step].y-points[step].x))));

		step++;
	}

	if(DEBUG && step>0) {
		printf("ITERATION: %d of %d\n",step-1,maxiters);
		printf("COLOR: R<%f>, G<%f> B<%f>\n",points[step-1].r,points[step-1].g,points[step-1].b);
		printf("OPACITY: %f", points[step-1].opacity);
	}

	glutPostRedisplay();
	glutTimerFunc(20,timerGL, 1);
}

GLuint createDL() {
	GLuint pointDL;
	pointDL = glGenLists(1);

	glNewList(pointDL,GL_COMPILE);
		draw3DPointGL();
	glEndList();

	return(pointDL);

}

void draw3DPointGL(){
	glColor4f(1.0f, 1.0f, 1.0f,opacity);

	glTranslatef(0.0f, 0.0f, 0.0f);
	glutSolidSphere(0.25f,5,5);
}

void orientGL()
{
	lx = sin(angle);
	lz = -cos(angle);
	ly = sin(angley);

	glLoadIdentity();
	gluLookAt(x, y, z, 
		      x + lx,y + ly,z + lz,
			  0.0f,1.0f,0.0f);
	if(DEBUG){
		printf("LookingAt: x(%f) y(%f) z(%f)\n",x,y,z);
		printf("Direction: lx(%f) ly(%f) lz(%f)\n",lx,ly,lz);
		printf("Angles: angle(%f) angley(%f)\n",angle,angley);
	}
}

void moveGL(
	int i
)
{
	x = x + i*(lx)*0.1;
	z = z + i*(lz)*0.1;
	y = y + i*(ly)*0.1;
	glLoadIdentity();
	gluLookAt(x, y, z, 
		      x + lx,y + ly,z + lz,
			  0.0f,1.0f,0.0f);
	if(DEBUG){
		printf("LookingAt: x(%f) y(%f) z(%f)\n",x,y,z);
		printf("Direction: lx(%f) ly(%f) lz(%f)\n",lx,ly,lz);
		printf("Angle: angle(%f) angley(%f)\n",angle,angley);
	}
}

void keyPressedGL(
	unsigned char key,
	int x, 
	int y
)
{
	int k = (int) key;
	switch (k) {
		case 113: case 81: case 27: // Q or ESC
			glutDestroyWindow(windowID);
			exit(EXIT_SUCCESS);
			break;
		case 90: case 122: // z or Z
			lorenzData.ro-=0.5;
			if(DEBUG)
				printf("RO: %f", lorenzData.ro);
			break;
		case 65: case 97: // a or A
			lorenzData.ro+=0.5;
			if(DEBUG)
				printf("RO: %f", lorenzData.ro);
			break;
		case 67: case 99: // c or C
			if(scheme>19)
				scheme=1;
			else
				scheme++;
			if(DEBUG)
				printf("Color Scheme: %d", scheme);
			break;
		case 83: case 115: // s or S
			if(style>2)
				style=1;
			else
				style++;
			if(DEBUG)
				printf("Color Scheme: %d", style);
			break;
		case 84: case 116: // t or T
			if(opacityUp){
				if(opacity>=1) {
					opacityUp=0;
					opacity-=0.05f;
				} else opacity+=0.05f;
			} else {
				if(opacity<=0) {
					opacityUp=1;
					opacity+=0.05f;
				} else opacity-=0.05f;
			}
			if(DEBUG)
				printf("Opacity: %f is %s\n", opacity,(opacityUp?"going up":"going down"));
			break;
		case 79: case 111: // o or O
			pixopflag = !pixopflag;
			break;
		case 32: //Space
			started=!started;
			break;
		default:
			if(DEBUG)
				printf ("WARNING: No action for %c key.\n", key);
			break;
	}
}

void specialKeyPressedGL(
	int key,
	int x, 
	int y
)
{
	switch (key) {
		case GLUT_KEY_LEFT:
			deltaAngle = -0.01f;
			if(DEBUG)
				printf ("DeltaAngle changed: %f\n", deltaAngle);
			break;
		case GLUT_KEY_RIGHT:
			deltaAngle = 0.01f;
			if(DEBUG)
				printf ("DeltaAngle changed: %f\n", deltaAngle);
			break;
		case GLUT_KEY_PAGE_UP:
			deltaMove = 1;
			break;
		case GLUT_KEY_PAGE_DOWN:
			deltaMove = -1;
			break;
		case GLUT_KEY_UP:
			deltaAngleY = 0.01f;
			if(DEBUG)
				printf ("DeltaAngleY changed: %f\n", deltaAngleY);
			break;
		case GLUT_KEY_DOWN:
			deltaAngleY = -0.01f;
			if(DEBUG)
				printf ("DeltaAngleY changed: %f\n", deltaAngleY);
			break;
		default:
			if(DEBUG)
				printf ("WARNING: No action for %c special-key.\n", key);
			break;
	}
}

void releaseKeyGL(
	int key,
	int x, 
	int y
)
{
	switch (key) {
		case GLUT_KEY_LEFT: case GLUT_KEY_RIGHT:
			deltaAngle = 0.0f;
			if(DEBUG)
				printf ("DeltaAngle changed: %f\n", deltaAngle);
			break;
		case GLUT_KEY_UP: case GLUT_KEY_DOWN:
			deltaAngleY = 0.0f;
			if(DEBUG)
				printf ("DeltaAngleY changed: %f\n", deltaAngleY);
			break;
		case GLUT_KEY_PAGE_UP: case GLUT_KEY_PAGE_DOWN:
			deltaMove = 0;
			if(DEBUG)
				printf ("DeltaMove changed: %d\n", deltaMove);
			break;
	}
}
