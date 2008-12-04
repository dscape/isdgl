#include "isdCore.h"

void drawGL();
void timerGL(int t);
void draw3DPointGL();
void showErrorGL(char * error);
void reshapeGL(GLsizei w, GLsizei h);
void initializeGL();
void keyPressedGL(unsigned char key, int x, int y);
void specialKeyPressedGL(int key, int x, int y);
void releaseKeyGL(int key, int x, int y);
void moveGL(int i);
void orientGL();
GLuint createDL();
