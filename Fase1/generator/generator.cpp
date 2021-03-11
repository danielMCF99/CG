#include<tuple>
#include<stdio.h>
#include<vector>
#include <iostream>
#include <fstream>
#include <math.h>
#include <string.h>
#include "../tinyxml2/tinyxml2.cpp"
#include "../tinyxml2/tinyxml2.h"


using namespace tinyxml2;
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


float RandomFloat(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

void trianglesToFile(vector<Triangle> t, string f) {
	ofstream file(f);
	unsigned int i = 0;
	while (i < t.size()) {
		file << "Triangle " << i << ":" << " " << (RandomFloat(0.0f,1.0f)) << " " << (RandomFloat(0.0f,1.0f)) << " " << (RandomFloat(0.0f,1.0f)) << " , " << get<0>(t[i].triangle).toString() << " , " << get<1>(t[i].triangle).toString() << " , " << get<2>(t[i].triangle).toString() << endl;
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
	Triangle t3(p1, p3, p2);
	Triangle t4(p2, p3 ,p4);

	vector<Triangle> v = { t1,t2,t3,t4 };

	string path = "../engine/draws/" + name;
	trianglesToFile(v, path);

}

void drawBox(float x, float y, float z, int w, string name) {

	float newx = x / (w + 1);
	float newz = z / (w + 1);
	float newy = y / (w + 1);

	vector<Triangle> triangles;

	for (float i = -(x / 2); i < x / 2; i += newx) {
		for (float j = -(z / 2); j < z / 2; j += newz) {
			// BASE
			Vertex p1(i, 0.0f, j);
			Vertex p2(i + newx, 0.0f, j);
			Vertex p3(i + newx, 0.0f, j + newz);
			Vertex p4(i, 0.0f, j + newz);

			Triangle t1(p1, p2, p3);
			Triangle t2(p3, p4, p1);

			triangles.push_back(t1);
			triangles.push_back(t2);

			// TOPO
			Vertex p5(i, y, j);
			Vertex p6(i + newx, y, j + newz);
			Vertex p7(i + newx, y, j);
			Vertex p8(i, y, j + newz);

			Triangle t3(p5, p6, p7);
			Triangle t4(p5, p8, p6);

			triangles.push_back(t3);
			triangles.push_back(t4);
		}
	}

	for (float i = -(x / 2); i < x / 2; i += newx) {
		for (float j = 0; j < y; j += newy) {
			//FRENTE
			Vertex p1(i, j, z / 2);
			Vertex p2(i + newx, j, z / 2);
			Vertex p3(i + newx, j + newy, z / 2);
			Vertex p4(i , j + newy, z / 2);
			
			Triangle t1(p1, p2, p3);
			Triangle t2(p1, p3, p4);

			triangles.push_back(t1);
			triangles.push_back(t2);

			// ATR�S
			Vertex p5(i, j, -z / 2);
			Vertex p6(i + newx, j + newy, -z / 2);
			Vertex p7(i + newx, j, -z / 2);
			Vertex p8(i, j + newy, -z / 2);
			
			Triangle t3(p5, p6, p7);
			Triangle t4(p5, p8, p6);

			triangles.push_back(t3);
			triangles.push_back(t4);

		}
	}

	for (float i = -(z / 2); i < z / 2; i += newz) {
		for (float j = 0; j < y; j += newy) {
			// FACE DA ESQUERDA
			Vertex p1(-x / 2, j, i);
			Vertex p2(-x / 2, j, i + newz);
			Vertex p3(-x / 2, j + newy, i);
			Vertex p4(-x / 2, j + newy, i + newz);

			Triangle t1(p1, p2, p3);
			Triangle t2(p2, p4, p3);

			triangles.push_back(t1);
			triangles.push_back(t2);

			// FACE DA DIREITA
			Vertex p5(x / 2, j, i);
			Vertex p6(x / 2, j + newy, i);
			Vertex p7(x / 2, j, i + newz);
			Vertex p8(x / 2, j + newy, i + newz);

			Triangle t3(p5, p6, p7);
			Triangle t4(p7, p6, p8);

			triangles.push_back(t3);
			triangles.push_back(t4);

		}
	}

	string path = "../engine/draws/" + name;
	trianglesToFile(triangles, path);
}

void drawSphere(float radius, int slices, int stacks, string name) {
	float slices_alpha = 2 * M_PI / (float)slices;
	float stacks_beta = M_PI / (float)stacks;
	vector<Triangle> triangles;

	for (int i = 0; i <= slices; i++) {
		float alpha = i * slices_alpha;
		for (int j = -stacks / 2; j <= stacks / 2; j++) {
			float beta = j * stacks_beta;

			Vertex p1(radius * cos(beta) * sin(alpha), radius * sin(beta), radius * cos(beta) * cos(alpha));
			Vertex p2(radius * cos(beta) * sin(alpha + slices_alpha), radius * sin(beta), radius * cos(beta) * cos(alpha + slices_alpha));
			Vertex p3(radius * cos(beta + stacks_beta) * sin(alpha + slices_alpha), radius * sin(beta + stacks_beta), radius * cos(beta + stacks_beta) * cos(alpha + slices_alpha));
			Vertex p4(radius * cos(beta + stacks_beta) * sin(alpha), radius * sin(beta + stacks_beta), radius * cos(beta + stacks_beta) * cos(alpha));

			Triangle t1(p1, p2, p3);
			Triangle t2(p1, p3, p4);
			
			triangles.push_back(t1);
			triangles.push_back(t2);
		}
	}
	string path = "../engine/draws/" + name;
	trianglesToFile(triangles, path);
}

void drawCone(float radius, float height, int slices, int stacks, string name) {
	float alpha = 2 * M_PI / (float)slices;
	float stackHeight = height / (float)stacks;
	float upper_radius, bottom_radius, upper, bottom;
	vector<Triangle> triangles;

	for (int i = 0; i <= slices; i++) {
		float delta_alpha = i * alpha;
		// BASE DO CONE
		Vertex p1(0.0f, 0.0f, 0.0f);
		Vertex p2(sin(delta_alpha) * radius, 0.0f, cos(delta_alpha) * radius);
		Vertex p3(sin(delta_alpha + alpha) * radius, 0.0f, cos(delta_alpha + alpha) * radius);

		Triangle t1(p3, p2, p1);
		triangles.push_back(t1);
	}

	for (int j = 0; j < stacks; j++) {

		bottom = j * stackHeight;
		bottom_radius = radius - ((radius / (float)stacks) * j);

		upper = (j + 1) * stackHeight;
		upper_radius = radius - ((radius / (float)stacks) * (j + 1));

		for (int i = 0; i < slices; i++) {
			float delta_alpha = i * alpha;

			Vertex p1(bottom_radius * sin(delta_alpha), bottom, bottom_radius * cos(delta_alpha));
			Vertex p2(upper_radius * sin(delta_alpha + alpha), upper, upper_radius * cos(delta_alpha + alpha));
			Vertex p3(upper_radius * sin(delta_alpha), upper, upper_radius * cos(delta_alpha));
			Vertex p4(bottom_radius * sin(delta_alpha + alpha), bottom, bottom_radius * cos(delta_alpha + alpha));

			Triangle t1(p1, p2, p3);
			Triangle t2(p1, p4, p2);
			triangles.push_back(t1);
			triangles.push_back(t2);
		}
	}

	string path = "../engine/draws/" + name;
	trianglesToFile(triangles, path);
}

void createXML() {
	XMLDocument doc;
	XMLElement* scene = doc.NewElement("scene");
	doc.LinkEndChild(scene);
	doc.SaveFile("../engine/cenarios/cenario1.xml");
}

void updateXML(const char* file) {
	XMLDocument doc;
	if (doc.LoadFile("../engine/cenarios/cenario1.xml") == XML_SUCCESS) {
		XMLElement* root = doc.RootElement();
		const char* atr;
		for (XMLElement* it = root->FirstChildElement(); it != NULL; it = it->NextSiblingElement()) {
			if (strcmp(file, it->Attribute("file")) == 0) {
				cout << "O ficheiro " << file << " ja existe no xml.\n";
				return;
			}
		}
		XMLElement* model = doc.NewElement("model");
		model->SetAttribute("file", file);
		root->LinkEndChild(model);
		doc.SaveFile("../engine/cenarios/cenario1.xml");
	}
	else {
		createXML();
		updateXML(file);
	}
}


int main(int argc, char* argv[]) {
	
	if ((strcmp(argv[1], "plane") == 0) && argc == 4) {
		drawPlane(atof(argv[2]), argv[3]);
		updateXML(argv[3]);
	}else if ((strcmp(argv[1], "box") == 0) && argc == 6) {
		drawBox(atof(argv[2]), atof(argv[3]), atof(argv[4]), 0 ,argv[5]);
		updateXML(argv[5]);
	}
	else if ((strcmp(argv[1], "box") == 0) && argc == 7) {
		drawBox(atof(argv[2]), atof(argv[3]), atof(argv[4]), atoi(argv[5]) , argv[6]);
		updateXML(argv[6]);
	}
	else if ((strcmp(argv[1], "cone") == 0) && argc == 7) {
		drawCone(atof(argv[2]), atof(argv[3]), atoi(argv[4]), atoi(argv[5]), argv[6]);
		updateXML(argv[6]);
	}
	else if ((strcmp(argv[1], "sphere") == 0) && argc == 6) {
		drawSphere(atof(argv[2]), atoi(argv[3]), atoi(argv[4]), argv[5]);
		updateXML(argv[5]);
	}

	return 0;
}

//	./generator plane 10 plane.3d

// ./generator box x y z size? box.3d

// ./generator cone bottom_radius height slices stacks cone.3d

// ./generator sphere 1 10 10 sphere.3d