#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "../tinyxml2/tinyxml2.cpp"
#include "../tinyxml2/tinyxml2.h"
#define _USE_MATH_DEFINES
#include <math.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace tinyxml2;
char* pathToXML = "../engine/cenario.xml";

std::vector<std::string> models;
std::vector<std::vector<float>> trianglesCoordinates;

GLfloat alpha, beta = 0.0f;
GLfloat r = 20.0f;

// Ler do ficheiro XML os ficheiros .3d e coloc�-los num vetor
void readFromXml() {
	XMLDocument file;

	file.LoadFile(pathToXML);

	// verifica se existe ficheiro ou n�o
	if (file.Error()) {
		std::cout << "O ficheiro nao existe";
		exit(0);
	}

	XMLElement* rootElement = file.RootElement();

	// vai a cada atributo com o atributo file e insere o nome do modelo no vetor models
	for (XMLElement* next = rootElement->FirstChildElement(); next != NULL; next = next->NextSiblingElement()) {
		std::string name = next->Attribute("file");

		models.push_back(name);
	}
}

void read3Dfile() {

	float x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12;
	
	for (std::vector<std::string>::iterator it = models.begin(); it != models.end(); ++it) {

		std::string path = "../engine/draws/" + *it;
		std::ifstream File(path);
		
		if (File.fail()) {
			std::cout << "O ficheiro nao existe" << std::endl;
			exit(-1);
		}

		std::string myText;
		char delimiter = ':';
		while (getline(File, myText)) {
			std::string token = myText.substr(myText.find(delimiter) + 1, myText.length());

			token.erase(std::remove(token.begin(), token.end(), ','), token.end());
			
			std::istringstream data(token);
			
			data >> x1 >> x2 >> x3 >> x4 >> x5 >> x6 >> x7 >> x8 >> x9 >> x10 >> x11 >> x12;
			
			std::vector<float> vf = { x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12 };

			trianglesCoordinates.push_back(vf);
		}
	}

}

void drawGL() {
	for (std::vector<std::vector<float>>::iterator it = trianglesCoordinates.begin(); it != trianglesCoordinates.end(); ++it) {
		glColor3f(it->at(0), it->at(1), it->at(2));

		for (int i = 3; i < 12; i+=3) {
			glVertex3f(it->at(i), it->at(i + 1), it->at(i + 2));
		}
	}
}

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

void drawSphere(float radius , int slices, int stacks) {
	float slices_alpha = 2 * M_PI / (float)slices;
	float stacks_beta = M_PI / (float)stacks;



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
	gluLookAt(r*sin(alpha)*cos(beta),r*sin(beta),r*cos(alpha)*cos(beta),
		      0.0,0.0,0.0,
			  0.0f,1.0f,0.0f);

// put the geometric transformations here


// put drawing instructions here
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	axis();
	readFromXml();
	read3Dfile();
	glBegin(GL_TRIANGLES);
	drawGL();
	glEnd();

	// End of frame
	glutSwapBuffers();
}


// write function to process keyboard events
void keyboard(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'w':
		beta += 15.0f;
		break;
	case 's':
		beta -= 15.0f;
		break;
	case 'a':
		alpha -= 15.0f;
		break;
	case 'd':
		alpha += 15.0f;
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
	glutIdleFunc(renderScene);
	
// put here the registration of the keyboard callbacks
	glutKeyboardFunc(keyboard);


//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
// enter GLUT's main cycle
	glutMainLoop();
	
	return 1;
}
