#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
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
char* pathToXML = (char*)"../engine/cenarios/cenario.xml";

GLfloat alpha = 0.0f, beta = 0.5f;
GLfloat r = 100.0f;

class Group {

public:
	std::vector<float> translation;
	std::vector<float> rotation;
	std::vector<float> scale;
	std::vector<std::vector<float>> trianglesCoordinates;

	Group(){
		translation = {0.0f , 0.0f , 0.0f};
		rotation = { 0.0f, 0.0f, 0.0f, 0.0f };
		scale = { 1.0f,1.0f,1.0f };
	}

	void setTranslation(float x , float y , float z) {
		translation[0] = x;
		translation[1] = y;
		translation[2] = z;
	}

	void setRotation(float angle, float x, float y, float z) {
		rotation[0] = angle;
		rotation[1] = x;
		rotation[2] = y;
		rotation[3] = z;
	}

	void setScale(float x, float y, float z) {
		scale[0] = x;
		scale[1] = y;
		scale[2] = z;
	}
	
	void read3Dfile(std::string name) {
		float x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12;
		std::string path = "../engine/draws/" + name;
		std::ifstream File(path);

		if (File.fail()) {
			std::cout << "O ficheiro " << name << " nao existe" << std::endl;
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

	void drawGL() {
		for (std::vector<std::vector<float>>::iterator it = trianglesCoordinates.begin(); it != trianglesCoordinates.end(); ++it) {
			glColor3f(it->at(0), it->at(1), it->at(2));
			for (int i = 3; i < 12; i += 3) {
				glVertex3f(it->at(i), it->at(i + 1), it->at(i + 2));
			}
		}
	}

};

class Scene {
public:
	std::vector<std::string> files; //nomes ficheiros 3d 
	std::unordered_map<std::string, std::vector<Group*>> data;

	void addFile(std::string filename, std::vector<float> trans, std::vector<float> rot, std::vector<float> scal) {
		Group* g = new Group();
		g->setTranslation(trans[0], trans[1], trans[2]);
		g->setRotation(rot[0], rot[1], rot[2], rot[3]);
		g->setScale(scal[0],scal[1],scal[2]);
		g->read3Dfile(filename);

		if (data.find(filename) != data.end()) {
			data[filename].push_back(g);
		}

		else {
			std::vector<Group*> lg;
			lg.push_back(g);
			data[filename] = lg;
		}
		files.push_back(filename);
	}

	void readFromXmlRec(XMLElement* element , std::vector<float> trans, std::vector<float> rot, std::vector<float> scal) {
		for (XMLElement* next = element; next != NULL; next = next->NextSiblingElement()) {
			if (strcmp(next->Name(), "rotate") == 0) {
				if (next->FindAttribute("angle")) {
					rot[0] += atof(next->FindAttribute("angle")->Value());
				}
				if (next->FindAttribute("X")) {
					rot[0] += atof(next->FindAttribute("X")->Value());
				}
				if (next->FindAttribute("Y")) {
					rot[1] += atof(next->FindAttribute("Y")->Value());
				}
				if (next->FindAttribute("Z")) {
					rot[2] += atof(next->FindAttribute("Z")->Value());
				}
			}

			else if (strcmp(next->Name(), "translate") == 0) {
				if (next->FindAttribute("X")) {
					trans[0] += atof(next->FindAttribute("X")->Value());
				}
				if (next->FindAttribute("Y")) {
					trans[1] += atof(next->FindAttribute("Y")->Value());
				}
				if (next->FindAttribute("Z")) {
					trans[2] += atof(next->FindAttribute("Z")->Value());
				}
			}

			else if (strcmp(next->Name(), "scale") == 0) {
				if (next->FindAttribute("X")) {
					scal[0] += atof(next->FindAttribute("X")->Value());
				}
				if (next->FindAttribute("Y")) {
					scal[1] += atof(next->FindAttribute("Y")->Value());
				}
				if (next->FindAttribute("Z")) {
					scal[2] += atof(next->FindAttribute("Z")->Value());
				}
			}

			else if (strcmp(next->Name(), "models") == 0) {
				for (XMLElement* model = next->FirstChildElement(); model != NULL; model = model->NextSiblingElement()) {
					std::string filename = model->Attribute("file");
					files.push_back(filename);
					addFile(filename, trans, rot, scal);
				}
			}

			else {
				readFromXmlRec(next, trans, rot, scal);
			}
		}
	}

	void readFromXml() {
		XMLDocument file;
		file.LoadFile(pathToXML);
		// verifica se existe ficheiro ou n�o
		if (file.Error()) {
			std::cout << "O ficheiro xml nao existe";
			exit(0);
		}
		XMLElement* rootElement = file.RootElement();
		// vai a cada atributo com o atributo file e insere o nome do modelo no vetor models
		for (XMLElement* next = rootElement->FirstChildElement(); next != NULL; next = next->NextSiblingElement()) {
			std::vector<float> trans = { 0.0f,0.0f,0.0f };
			std::vector<float> rot = { 0.0f, 0.0f, 0.0f, 0.0f };
			std::vector<float> scal = { 1.0f , 1.0f, 1.0f };
			readFromXmlRec(next, trans, rot, scal);
		}
	}

	void draw() {
		for (auto aux : data) {
			for (auto it : aux.second) {
				it->drawGL();
			}
		}
	}
};



void axis() {
	glBegin(GL_LINES);
	// X axis in red
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-200.0f, 0.0f, 0.0f);
	glVertex3f(200.0f, 0.0f, 0.0f);
	// Y Axis in Green
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, -200.0f, 0.0f);
	glVertex3f(0.0f, 200.0f, 0.0f);
	// Z Axis in Blue
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, -200.0f);
	glVertex3f(0.0f, 0.0f, 200.0f);
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

Scene* scene = new Scene();
void renderScene(void) {

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(r * sin(alpha) * cos(beta), r * sin(beta), r * cos(alpha) * cos(beta),
		0.0, 0.0, 0.0,
		0.0f, 1.0f, 0.0f);

// put the geometric transformations here


// put drawing instructions here
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	axis();
	glBegin(GL_TRIANGLES);
	scene->draw();
	glEnd();

	// End of frame
	glutSwapBuffers();
}


// write function to process keyboard events
void keyboard(unsigned char key, int x, int y) {
	switch (toupper(key))
	{
	case 'W':
		beta += 0.1f;
		if (beta > 1.5f)
			beta = 1.5f;
		break;
	case 'S':
		beta -= 0.1f;
		if (beta < -1.5f)
			beta = -1.5f;
		break;
	case 'A':
		alpha -= 0.1f;
		break;
	case 'D':
		alpha += 0.1f;
		break;
	case 'R':
		scene->readFromXml();
		//scene->read3Dfile();
		break;
	default:
		break;
	};
	glutPostRedisplay();
}


int main(int argc, char **argv) {
	scene->readFromXml();
	//scene->read3Dfile();
// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(1000,1000);
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
