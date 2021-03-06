#include<tuple>
#include<stdio.h>
#include<vector>
#include <iostream>
#include <fstream>
#include <math.h>
#include <string.h>
//#include "../tinyxml2-master/tinyxml2.h"
//#include "../tinyxml2-master/tinyxml2.cpp"

//using namespace tinyxml2;
using namespace std;

class Vertex {

public:
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	Vertex() {
		
	}

	Vertex(float x1, float y1, float z1) {
		x = x1;
		y = y1;
		z = z1;
	}

	string toString() {
		char* buffer = (char*)malloc(sizeof(char) * 100);
		snprintf(buffer, 100, "%.5f %.5f %.5f", x, y, z);
		return buffer;
	}
};

class Triangle{

public:
	tuple <Vertex, Vertex, Vertex> triangle; 

	Triangle() {
		Vertex p(0.0f, 0.0f, 0.0f);

		triangle = make_tuple(p, p, p);
	}
	
	Triangle(Vertex a , Vertex b , Vertex c) {
		triangle = make_tuple(a, b, c);
	}
};

void trianglesToFile(vector<Triangle> t, string f) {
	ofstream file(f);
	unsigned int i = 0;
	while (i < t.size()) {
		file << "Triangle " << i << ":" << get<0>(t[i].triangle).toString() << "," << get<1>(t[i].triangle).toString() << "," << get<2>(t[i].triangle).toString() << endl;
		i++;
	}
}

void drawPlane(float size, string name) {

	float half = size / 2;

	Vertex p1(half, 0.0f, half);
	Vertex p2(half, 0.0f, -half);
	Vertex p3(-half, 0.0f, half);
	Vertex p4(-half, 0.0f, -half);

	Triangle t1(p1, p2, p3);
	Triangle t2(p2, p4, p3);

	vector<Triangle> v = { t1,t2 };

	trianglesToFile(v, name);

}

/*void createXML() {
	XMLDocument doc;
	XMLElement* Scene = doc.NewElement("Scene");
	doc.LinkEndChild(Scene);
	doc.SaveFile("../Files.xml");
}*/

int main(int argc, char* argv[]) {
	
	if ((strcmp(argv[1], "plane") == 0) && argc == 4) {
		drawPlane(atof(argv[2]), argv[3]);
		//escrever no XML: <model file=”plane.3d” />
	}

	return 0;
}

//./generator plane 10 plane.3d