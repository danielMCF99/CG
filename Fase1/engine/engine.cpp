#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define _USE_MATH_DEFINES

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

GLfloat alpha, beta = 0.0f;
GLfloat r = 10.0f;

void axis() {
	glBegin(GL_LINES);
	// X axis in red
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-100.0f, 0.0f, 0.0f);
	glVertex3f(100.0f, 0.0f, 0.0f);
	// Y Axis in Green
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, -100.0f, 0.0f);
	glVertex3f(0.0f, 100.0f, 0.0f);
	// Z Axis in Blue
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, -100.0f);
	glVertex3f(0.0f, 0.0f, 100.0f);
	glEnd();

}

void drawBox(float x, float y, float z, float w) {
	
	float halfx = x / 2;
	float halfz = z / 2;
	
	// BASE
	glBegin(GL_TRIANGLES);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-halfx, 0.0f, halfz);
		glVertex3f(halfx, 0.0f, -halfz);
		glVertex3f(halfx, 0.0f, halfz);

		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-halfx, 0.0f, halfz);
		glVertex3f(-halfx, 0.0f, -halfz);
		glVertex3f(halfx, 0.0f, -halfz);
		
	glEnd();

	// TOPO
	glBegin(GL_TRIANGLES);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(halfx, y, halfz);
		glVertex3f(halfx, y, -halfz);
		glVertex3f(-halfx, y, halfz);

		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(halfx, y, -halfz);
		glVertex3f(-halfx, y, -halfz);
		glVertex3f(-halfx, y, halfz);
	glEnd();

	// DIREITA
	glBegin(GL_TRIANGLES);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(halfx, 0, halfz);
		glVertex3f(halfx, 0, -halfz);
		glVertex3f(halfx, y, halfz);

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(halfx, 0, -halfz);
		glVertex3f(halfx, y, -halfz);
		glVertex3f(halfx, y, halfz);
	glEnd();

	// FRENTE
	glBegin(GL_TRIANGLES);
		glColor3f(0.5f, 0.5f, 0.0f);
		glVertex3f(-halfx, 0, halfz);
		glVertex3f(halfx, 0, halfz);
		glVertex3f(halfx, y, halfz);

		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f(-halfx, y, halfz);
		glVertex3f(-halfx, 0, halfz);
		glVertex3f(halfx, y, halfz);
	glEnd();

	// Esquerda
	glBegin(GL_TRIANGLES);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-halfx, y, halfz);
		glVertex3f(-halfx, 0, halfz);
		glVertex3f(-halfx, 0, -halfz);

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-halfx, 0, -halfz);
		glVertex3f(-halfx, y, -halfz);
		glVertex3f(-halfx, y, halfz);
	glEnd();

	// ATRÁS
	glBegin(GL_TRIANGLES);
		glColor3f(0.5f, 0.5f, 0.0f);
		glVertex3f(-halfx, 0, -halfz);
		glVertex3f(halfx, 0, -halfz);
		glVertex3f(halfx, y, -halfz);

		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f(-halfx, y, -halfz);
		glVertex3f(-halfx, 0, -halfz);
		glVertex3f(halfx, y, -halfz);
	glEnd();

}



void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if(h == 0)
		h = 1;

	// compute window's aspect ratio 
	float ratio = w * 1.0 / h;

	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load Identity Matrix
	glLoadIdentity();
	
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set perspective
	gluPerspective(45.0f ,ratio, 1.0f ,1000.0f);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}


void renderScene(void) {

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(5.0,5.0,5.0,
		      0.0,0.0,0.0,
			  0.0f,1.0f,0.0f);

// put the geometric transformations here


// put drawing instructions here
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	axis();
	drawBox(3.0f,2.0f,3.0f,0.0f);

	// End of frame
	glutSwapBuffers();
}


// write function to process keyboard events
void keyboard(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'w':
		beta += 0.1f;
		break;
	case 's':
		beta -= 0.1f;
		break;
	case 'a':
		alpha += 0.1f;
		break;
	case 'd':
		alpha += 0.1f;
		break;
	default:
		break;
	};
	glutPostRedisplay();
}


int main(int argc, char **argv) {
// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(800,800);
	glutCreateWindow("CG@DI-UM");
		
// Required callback registry 
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);

	
// put here the registration of the keyboard callbacks
	glutKeyboardFunc(keyboard);


//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
// enter GLUT's main cycle
	glutMainLoop();
	
	return 1;
}
