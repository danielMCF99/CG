#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include "../tinyxml2/tinyxml2.cpp"
#include "../tinyxml2/tinyxml2.h"
#include <algorithm>
#include <cctype>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>

using namespace tinyxml2;
char* pathToXML = (char*)"../engine/cenarios/cenario.xml";

GLfloat alpha = 0.0f, beta = 0.5f;
GLfloat r = 170.0f;

float camX = 00, camY = 30, camZ = 40;
int startX, startY, tracking = 0;

int timebase = 0, frame = 0;
float Y[3] = { 0.0f, 1.0f, 0.0f };
float tesselation = 100;


GLuint buffers;


void buildRotMatrix(float* x, float* y, float* z, float* m) {

	m[0] = x[0]; m[1] = x[1]; m[2] = x[2]; m[3] = 0;
	m[4] = y[0]; m[5] = y[1]; m[6] = y[2]; m[7] = 0;
	m[8] = z[0]; m[9] = z[1]; m[10] = z[2]; m[11] = 0;
	m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
}


void cross(float* a, float* b, float* res) {

	res[0] = a[1] * b[2] - a[2] * b[1];
	res[1] = a[2] * b[0] - a[0] * b[2];
	res[2] = a[0] * b[1] - a[1] * b[0];
}


void normalize(float* a) {

	float l = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
	a[0] = a[0] / l;
	a[1] = a[1] / l;
	a[2] = a[2] / l;
}


void multMatrixVector(float* m, float* v, float* res) {

	for (int j = 0; j < 4; ++j) {
		res[j] = 0;
		for (int k = 0; k < 4; ++k) {
			res[j] += v[k] * m[j * 4 + k];
		}
	}

}


void getCatmullRomPoint(float t, float* p0, float* p1, float* p2, float* p3, float* pos, float* deriv) {
	int i;
	// catmull-rom matrix
	float m[4][4] = { {-0.5f,  1.5f, -1.5f,  0.5f},
						{ 1.0f, -2.5f,  2.0f, -0.5f},
						{-0.5f,  0.0f,  0.5f,  0.0f},
						{ 0.0f,  1.0f,  0.0f,  0.0f} };

	for (i = 0; i < 3; i++) {
		float vectorA[4] = { 0,0,0,0 };
		float vectorP[4] = { p0[i],p1[i],p2[i],p3[i] };
		multMatrixVector(*m, vectorP, vectorA);

		float vectorT[4] = { powf(t,3),powf(t,2),powf(t,1),1 };
		float vectordT[4] = { 3 * powf(t,2), 2 * t, 1, 0 };

		pos[i] = (vectorT[0] * vectorA[0]) + (vectorT[1] * vectorA[1]) + (vectorT[2] * vectorA[2]) + (vectorT[3] * vectorA[3]);
		deriv[i] = (vectordT[0] * vectorA[0]) + (vectordT[1] * vectorA[1]) + (vectordT[2] * vectorA[2]) + (vectordT[3] * vectorA[3]);

	}
}


/// given  global t, returns the point in the curve
void getGlobalCatmullRomPoint(float gt, float* pos, float* deriv, std::vector<std::vector<float>> basePoints) {
	int POINT_COUNT = basePoints.size();
	float t = gt * POINT_COUNT; // this is the real global t
	int index = floor(t);  // which segment
	t = t - index; // where within  the segment

	// indices store the points
	int indices[4];
	indices[0] = (index + POINT_COUNT - 1) % POINT_COUNT;
	indices[1] = (indices[0] + 1) % POINT_COUNT;
	indices[2] = (indices[1] + 1) % POINT_COUNT;
	indices[3] = (indices[2] + 1) % POINT_COUNT;

	getCatmullRomPoint(t, (float*)basePoints[indices[0]].data(), (float*)basePoints[indices[1]].data(), (float*)basePoints[indices[2]].data(), (float*)basePoints[indices[3]].data(), pos, deriv);
}

void renderCatmullRomCurve(std::vector< std::vector< float >> basePoints) {

	// draw curve using line segments with GL_LINE_LOOP
	float pos[3], deriv[3];

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < tesselation; i++) {
		getGlobalCatmullRomPoint(i / tesselation , pos, deriv, basePoints);
		glVertex3f(pos[0], pos[1], pos[2]);
	}
	glEnd();
}

class Group {

public:
	std::vector<std::vector<std::vector<float>>> translation;
	std::vector<float> rotation;
	std::vector<float> scale;
	std::vector<float> vbo;
	std::vector<std::vector<float>> triangleCoordinates;
	std::vector<float> colors;
	std::vector<float> time = { 1,1,1,1,1 };
	std::vector<float> elapsedTime = { 0,0,0,0 };
	int pos;
	float rotationTime;
	float angle = 0.0f;

	Group(){
		rotation = { 0.0f, 0.0f, 0.0f };
		scale = { 1.0f,1.0f,1.0f };
	}



	void setRotation(float x, float y, float z) {
		rotation[0] = x;
		rotation[1] = y;
		rotation[2] = z;
	}

	void setScale(float x, float y, float z) {
		scale[0] = x;
		scale[1] = y;
		scale[2] = z;
	}

	void setPos(int p) {
		pos = p;
	}
	
	void read3Dfile(std::string name) {
		float x1, x2, x3, x4, x5, x6, x7, x8, x9;
		std::string path = "../engine/draws/" + name;
		std::ifstream File(path);

		if (File.fail()) {
			std::cout << "O ficheiro " << name << " nao existe" << std::endl;
			exit(-1);
		}



		std::string myText;
		getline(File, myText);
		int vertexCount = atoi(myText.c_str());
		char delimiter = ':';
		while (getline(File, myText)) {
			std::string token = myText.substr(myText.find(delimiter) + 1, myText.length());

			token.erase(std::remove(token.begin(), token.end(), ','), token.end());

			std::istringstream data(token);
			
			data >> x1 >> x2 >> x3 >> x4 >> x5 >> x6 >> x7 >> x8 >> x9;
			std::vector<float> vf = { x1,x2,x3,x4,x5,x6,x7,x8,x9 };

			triangleCoordinates.push_back(vf);

			vbo.push_back(x1);
			vbo.push_back(x2);
			vbo.push_back(x3);

			vbo.push_back(x4);
			vbo.push_back(x5);
			vbo.push_back(x6);

			vbo.push_back(x7);
			vbo.push_back(x8);
			vbo.push_back(x9);
		}
	}
};

class Scene {
public:
	std::vector<std::string> files;
	std::unordered_map<std::string, std::vector<Group*>> data;

	void addFile(std::string filename, std::vector<std::vector<std::vector<float>>> trans, std::vector<float> rot, std::vector<float> scal, std::vector<float> t,std::vector<float> col,int pos,float rotT) {
		Group* g = new Group();
		g->translation = trans;
		g->setRotation(rot[0], rot[1], rot[2]);
		g->setScale(scal[0],scal[1],scal[2]);
		g->read3Dfile(filename);
		g->time = t;
		g->setPos(pos);
		g->rotationTime = rotT;
		g->colors = col;
		if (rotT != 1) {
			g->angle = 360 / rotT;
		}

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

	void readFromXmlRec(XMLElement* element , std::vector<std::vector<std::vector<float>>> trans, std::vector<float> rot, std::vector<float> scal, std::vector<float> time, std::vector<float> col ,int pos,float rotT) {
		for (XMLElement* next = element; next != NULL; next = next->NextSiblingElement()) {


			if (strcmp(next->Name(), "colour") == 0) {
				if (next->FindAttribute("red")) {
					col[0] = atof(next->FindAttribute("red")->Value());
				}
				if (next->FindAttribute("green")) {
					col[1] = atof(next->FindAttribute("green")->Value());
				}
				if (next->FindAttribute("blue")) {
					col[2] = atof(next->FindAttribute("blue")->Value());
				}
			}


			else if (strcmp(next->Name(), "rotate") == 0) {
				if (next->FindAttribute("time")) {
					rotT = atof(next->FindAttribute("time")->Value());
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

				time[pos+1] = atof(next->FindAttribute("time")->Value()) > 0 ? atof(next->FindAttribute("time")->Value()) : 1000;

				std::vector<std::vector<float>> tstate;
				std::vector<float> points;

				for (auto tag = next->FirstChildElement(); tag != NULL; tag = tag->NextSiblingElement()) {

					std::vector<float> points = { tag->FindAttribute("X") ? (float)atof(tag->FindAttribute("X")->Value()) : 0, tag->FindAttribute("Y") ? (float)atof(tag->FindAttribute("Y")->Value()) : 0, tag->FindAttribute("Z") ? (float)atof(tag->FindAttribute("Z")->Value()) : 0 };

					tstate.push_back(points);
				}
				trans.push_back(tstate);
				pos++;
			}

			else if (strcmp(next->Name(), "scale") == 0) {

				if (next->FindAttribute("X")) {
					scal[0] *= atof(next->FindAttribute("X")->Value());
				}
				if (next->FindAttribute("Y")) {
					scal[1] *= atof(next->FindAttribute("Y")->Value());
				}
				if (next->FindAttribute("Z")) {
					scal[2] *= atof(next->FindAttribute("Z")->Value());
				}
			}

			else if (strcmp(next->Name(), "models") == 0) {
				for (XMLElement* model = next->FirstChildElement(); model != NULL; model = model->NextSiblingElement()) {
					std::string filename = model->Attribute("file");
					files.push_back(filename);
					addFile(filename, trans, rot, scal, time, col, pos, rotT);
				}
			}

			else if (strcmp(next->Name(), "group") == 0) {
				readFromXmlRec(next->FirstChildElement(), trans, rot, scal, time, col, pos, rotT);

			}

			else {

				std::cout << "Comando Desconhecido" << std::endl;

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
			std::vector<std::vector<std::vector<float>>> trans;
			std::vector<float> rot = { 0.0f, 0.0f, 0.0f };
			std::vector<float> scal = { 1.0f , 1.0f, 1.0f };
			std::vector<float> time = { 1.0f,1.0f,1.0f,1.0f,1.0f };
			std::vector<float> col = { 0.5f,0.5f,0.5f };
			int pos = -1;
			float rotT = 1;
			readFromXmlRec(next, trans, rot, scal, time, col, pos, rotT);
		}
	}

	void draw() {
		float position[3], derivative[3] = { 0.0f,0.0f,0.0f };
		float M[16];
		float X[3], Z[3];
		for (auto itr = data.begin(); itr != data.end(); itr++) {
			std::string key = itr->first;
			for (auto aux : data[key]) {
				glColor3f(aux->colors[0]/255, aux->colors[1]/255, aux->colors[2]/255);
				glPushMatrix();
					for (int i = 0; i <= aux->pos ; i++) {
						renderCatmullRomCurve(aux->translation[i]);
						getGlobalCatmullRomPoint(aux->elapsedTime[i] + (1.0 / aux->time[i]), position, derivative, aux->translation[i]);
						glTranslatef(position[0], position[1], position[2]);
						aux->elapsedTime[i] += (1.0 / aux->time[i]);

						if (aux->rotationTime > 1) {
							X[0] = derivative[0];
							X[1] = derivative[1];
							X[2] = derivative[2];

							cross(X, Y, Z);
							cross(Z, X, Y);
							normalize(X);
							normalize(Y);
							normalize(Z);

							buildRotMatrix(X, Y, Z, M);
							glMultMatrixf(M);
						}
					}



					if (aux->angle != 0) {
						glRotatef(aux->angle, aux->rotation[0], aux->rotation[1], aux->rotation[2]);
						aux->angle += 360 / aux->rotationTime;
					}

					glScalef(aux->scale[0], aux->scale[1], aux->scale[2]);
					
					glBindBuffer(GL_ARRAY_BUFFER, buffers);
					glBufferData(GL_ARRAY_BUFFER, sizeof(float) * aux->vbo.size(), aux->vbo.data(), GL_STATIC_DRAW);
					glVertexPointer(3, GL_FLOAT, 0, 0);
					glDrawArrays(GL_TRIANGLES, 0, ((GLuint)aux->vbo.size() / 3));
				glPopMatrix();

			}
		}
	}
};



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
	float fps;
	int time;
	char s[64];


	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt((float)r * sin(alpha) * cos(beta),(float)r * sin(beta), (float)r * cos(alpha) * cos(beta),
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);

// put the geometric transformations here

// put drawing instructions here
	scene->draw();
	frame++;
	time = glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000) {
		fps = frame * 1000.0 / (time - timebase);
		timebase = time;
		frame = 0;
		sprintf(s, "FPS: %f6.2", fps);
		glutSetWindowTitle(s);
	}

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
	case 'Z':
		r += 5.0f;
		break;
	case 'X':
		r -= 5.0f;
		break;
	case 'R':
		scene->readFromXml();
		break;
	case 'P':
		glPolygonMode(GL_FRONT, GL_FILL);
		break;
	case 'O':
		glPolygonMode(GL_FRONT, GL_LINE);
		break;
	case 'I':
		glPolygonMode(GL_FRONT, GL_POINT);
		break;
	case 'M':
		tesselation -= 50;
		break;
	case 'N':
		tesselation += 50;
		break;
	case 'F':
		glutFullScreen();
		break;
	case 'G':
		glutPositionWindow(100, 100);
		glutReshapeWindow(800, 600);
		break;
	default:
		break;
	};
	glutPostRedisplay();
}


int main(int argc, char **argv) {
	scene->readFromXml();
// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(1000,1000);
	glutCreateWindow("Solar System");
		
// Required callback registry 
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);
	
// put here the registration of the keyboard callbacks
	glutKeyboardFunc(keyboard);

	#ifndef __APPLE__	
	// init GLEW
	glewInit();
	#endif	

	// OpenGL settings 
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glGenBuffers(1, &buffers);
	
// enter GLUT's main cycle
	glutMainLoop();
	
	return 1;
}
