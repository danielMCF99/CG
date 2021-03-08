#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "../tinyxml2/tinyxml2.cpp"
#include "../tinyxml2/tinyxml2.h"
#define _USE_MATH_DEFINES

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
GLfloat r = 10.0f;

// Ler do ficheiro XML os ficheiros .3d e colocá-los num vetor
void readFromXml() {
	XMLDocument file;

	file.LoadFile(pathToXML);

	// verifica se existe ficheiro ou não
	if (file.Error()) {
		std::cout << "O ficheiro NAO existe";
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

	float x1, x2, x3, x4, x5, x6, x7, x8, x9;
	
	for (std::vector<std::string>::iterator it = models.begin(); it != models.end(); ++it) {

		std::string path = "../engine/" + *it;
		std::ifstream File(path);
		
		if (File.fail()) {
			std::cout << "O ficheiro nao existe";
			exit(0);
		}

		std::string myText;
		char delimiter = ':';
		while (getline(File, myText)) {
			std::string token = myText.substr(myText.find(delimiter) + 1, myText.length());

			token.erase(std::remove(token.begin(), token.end(), ','), token.end());
			
			std::istringstream data(token);
			
			data >> x1 >> x2 >> x3 >> x4 >> x5 >> x6 >> x7 >> x8 >> x9;
			

			std::vector<float> vf = { x1,x2,x3,x4,x5,x6,x7,x8,x9 };

			trianglesCoordinates.push_back(vf);
		}
	}

}

void drawGL() {
	float c1, c2, c3;
	for (std::vector<std::vector<float>>::iterator it = trianglesCoordinates.begin(); it != trianglesCoordinates.end(); ++it) {
		c1 = rand() % 3;
		c2 = rand() % 3;
		c3 = rand() % 3;
		glColor3f(c1, c2, c3);
		for (int i = 0; i < 9; i+=3) {
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
		beta += 0.1f;
		break;
	case 's':
		beta -= 0.1f;
		break;
	case 'a':
		alpha -= 0.1f;
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
