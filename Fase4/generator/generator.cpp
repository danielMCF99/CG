#include<tuple>
#include<stdio.h>
#include<vector>
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<math.h>
#include "../tinyxml2/tinyxml2.cpp"
#include "../tinyxml2/tinyxml2.h"

#define _USE_MATH_DEFINES

using namespace tinyxml2;
using namespace std;


void removeChar(std::string& str, char character)
{
    size_t pos;
    while ((pos = str.find(character)) != std::string::npos)
        str[pos] = ' ';
}


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

void normalize(Vertex n) {
    float l = sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
    n.x = n.x / l;
    n.y = n.y / l;
    n.z = n.z / l;
}

float RandomFloat(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

void trianglesToFile(vector<Triangle> t, string f) {
    ofstream file(f);
    unsigned int i = 0;
    int size = t.size() * 3;
    //file << size << endl;
    while (i < t.size()) {
        file << "Triangle " << i << ":" << " "  << get<0>(t[i].triangle).toString() << " , " << get<1>(t[i].triangle).toString() << " , " << get<2>(t[i].triangle).toString() << endl;
        i++;
    }
}

void normalsToFile(vector<Triangle> normals, string f) {
    ofstream file(f, ofstream::app);
    unsigned int i = 0;
    int size = normals.size() * 3;
    //file << size << endl;
    file << "Normals" << endl;
    while (i < normals.size()) {
        file << "Normal " << i << ":" << " " << get<0>(normals[i].triangle).toString() << " , " << get<1>(normals[i].triangle).toString() << " , " << get<2>(normals[i].triangle).toString() << endl;
        i++;
    }
}

void drawPlane(float size, string name) {

    float half = size / 2;

    Vertex norm1(0.0f, 1.0f, 0.0f);
    Vertex norm2(0.0f, -1.0f, 0.0f);

    Triangle n1( norm1,norm1,norm1 );
    Triangle n2( norm2,norm2,norm2 );

    Vertex p1(half, 0.0f, half);
    Vertex p2(half, 0.0f, -half);
    Vertex p3(-half, 0.0f, half);
    Vertex p4(-half, 0.0f, -half);

    Triangle t1(p1, p2, p3);
    Triangle t2(p2, p4, p3);
    Triangle t3(p1, p3, p2);
    Triangle t4(p2, p3 ,p4);

    vector<Triangle> v = { t1,t2,t3,t4 };
    vector<Triangle> n = { n1,n1,n2,n2 };

    string path = "../engine/draws/" + name;
    trianglesToFile(v, path);
    normalsToFile(n, path);
}

void drawBox(float x, float y, float z, int w, string name) {

    float newx = x / (w + 1);
    float newz = z / (w + 1);
    float newy = y / (w + 1);

    vector<Triangle> triangles;
    vector<Triangle> normals;

    for (float i = -(x / 2); i < x / 2; i += newx) {
        for (float j = -(z / 2); j < z / 2; j += newz) {
            // BASE
            Vertex norm1(0.0f, -1.0f, 0.0f);
            Triangle n1{ norm1,norm1,norm1 };

            Vertex p1(i, 0.0f, j);
            Vertex p2(i + newx, 0.0f, j);
            Vertex p3(i + newx, 0.0f, j + newz);
            Vertex p4(i, 0.0f, j + newz);

            Triangle t1(p1, p2, p3);
            Triangle t2(p3, p4, p1);

            triangles.push_back(t1);
            triangles.push_back(t2);

            normals.push_back(n1);
            normals.push_back(n1);

            // TOPO
            Vertex norm2(0.0f, 1.0f, 0.0f);
            Triangle n2( norm2, norm2, norm2 );

            Vertex p5(i, y, j);
            Vertex p6(i + newx, y, j + newz);
            Vertex p7(i + newx, y, j);
            Vertex p8(i, y, j + newz);

            Triangle t3(p5, p6, p7);
            Triangle t4(p5, p8, p6);

            triangles.push_back(t3);
            triangles.push_back(t4);

            normals.push_back(n2);
            normals.push_back(n2);
        }
    }

    for (float i = -(x / 2); i < x / 2; i += newx) {
        for (float j = 0; j < y; j += newy) {
            //FRENTE
            Vertex norm3(0.0f, 0.0f, 1.0f);
            Triangle n3{ norm3,norm3,norm3 };

            Vertex p1(i, j, z / 2);
            Vertex p2(i + newx, j, z / 2);
            Vertex p3(i + newx, j + newy, z / 2);
            Vertex p4(i , j + newy, z / 2);

            Triangle t1(p1, p2, p3);
            Triangle t2(p1, p3, p4);

            triangles.push_back(t1);
            triangles.push_back(t2);

            normals.push_back(n3);
            normals.push_back(n3);

            // ATRï¿½S
            Vertex norm4(0.0f, 0.0f, -1.0f);
            Triangle n4{ norm4,norm4,norm4 };

            Vertex p5(i, j, -z / 2);
            Vertex p6(i + newx, j + newy, -z / 2);
            Vertex p7(i + newx, j, -z / 2);
            Vertex p8(i, j + newy, -z / 2);

            Triangle t3(p5, p6, p7);
            Triangle t4(p5, p8, p6);

            triangles.push_back(t3);
            triangles.push_back(t4);

            normals.push_back(n4);
            normals.push_back(n4);

        }
    }

    for (float i = -(z / 2); i < z / 2; i += newz) {
        for (float j = 0; j < y; j += newy) {
            // FACE DA ESQUERDA
            Vertex norm5(-1.0f, 0.0f, 0.0f);
            Triangle n5 (norm5,norm5,norm5 );

            Vertex p1(-x / 2, j, i);
            Vertex p2(-x / 2, j, i + newz);
            Vertex p3(-x / 2, j + newy, i);
            Vertex p4(-x / 2, j + newy, i + newz);

            Triangle t1(p1, p2, p3);
            Triangle t2(p2, p4, p3);

            triangles.push_back(t1);
            triangles.push_back(t2);

            normals.push_back(n5);
            normals.push_back(n5);

            // FACE DA DIREITA
            Vertex norm6(1.0f, 0.0f, 0.0f);
            Triangle n6( norm6,norm6,norm6 );

            Vertex p5(x / 2, j, i);
            Vertex p6(x / 2, j + newy, i);
            Vertex p7(x / 2, j, i + newz);
            Vertex p8(x / 2, j + newy, i + newz);

            Triangle t3(p5, p6, p7);
            Triangle t4(p7, p6, p8);

            triangles.push_back(t3);
            triangles.push_back(t4);

            normals.push_back(n6);
            normals.push_back(n6);
        }
    }

    string path = "../engine/draws/" + name;
    trianglesToFile(triangles, path);
    normalsToFile(normals, path);
}

void drawSphere(float radius, int slices, int stacks, string name) {
    float slices_alpha = 2 * M_PI / (float)slices;
    float stacks_beta = M_PI / (float)stacks;
    vector<Triangle> triangles;
    vector<Triangle> normals;

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

            Vertex n1(p1.x, p1.y, p1.z);
            Vertex n2(p2.x, p2.y, p2.z);
            Vertex n3(p3.x, p3.y, p3.z);
            Vertex n4(p4.x, p4.y, p4.z);

            normalize(n1);
            normalize(n2);
            normalize(n3);
            normalize(n4);

            Triangle tn1(n1,n2,n3);
            Triangle tn2(n1,n3,n4);

            normals.push_back(tn1);
            normals.push_back(tn2);

            triangles.push_back(t1);
            triangles.push_back(t2);
        }
    }
    string path = "../engine/draws/" + name;
    trianglesToFile(triangles, path);
    normalsToFile(normals,path);
}

void drawCone(float radius, float height, int slices, int stacks, string name) {
    float alpha = 2 * M_PI / (float)slices;
    float stackHeight = height / (float)stacks;
    float upper_radius, bottom_radius, upper, bottom;
    vector<Triangle> triangles;
    vector<Triangle> normals;

    for (int i = 0; i <= slices; i++) {
        float delta_alpha = i * alpha;
        // BASE DO CONE
        Vertex p1(0.0f, 0.0f, 0.0f);
        Vertex p2(sin(delta_alpha) * radius, 0.0f, cos(delta_alpha) * radius);
        Vertex p3(sin(delta_alpha + alpha) * radius, 0.0f, cos(delta_alpha + alpha) * radius);

        Vertex n0(0, -1, 0);

        Triangle tn0(n0, n0, n0);

        normals.push_back(tn0);

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

            Vertex n1(p1.x, p1.y, p1.z);
            Vertex n2(p2.x, p2.y, p2.z);
            Vertex n3(p3.x, p3.y, p3.z);
            Vertex n4(p4.x, p4.y, p4.z);

            normalize(n1);
            normalize(n2);
            normalize(n3);
            normalize(n4);

            Triangle tn1(n1,n2,n3);
            Triangle tn2(n1,n3,n4);

            normals.push_back(tn1);
            normals.push_back(tn2);

            triangles.push_back(t1);
            triangles.push_back(t2);

        }
    }

    string path = "../engine/draws/" + name;
    trianglesToFile(triangles, path);
}


void drawTorus(float outerRadius, float radius, int slices, int stacks,string name) {

    float theta = 0;
    float phi = 0;
    float theta_shift = (2 * M_PI) / slices;
    float phi_shift = (2 * M_PI) / stacks;
    vector<Triangle> triangles;

    for (int i = 0; i < slices; i++) {
        for (int j = 0; j < stacks; j++) {
            Vertex v1((cos(theta) * (outerRadius + radius * cos(phi))), (sin(theta) * (outerRadius + radius * cos(phi))), (radius * sin(phi)));
            Vertex v2((cos(theta + theta_shift) * (outerRadius + radius * cos(phi))), (sin(theta + theta_shift) * (outerRadius + radius * cos(phi))), (radius * sin(phi)));
            Vertex v3((cos(theta + theta_shift) * (outerRadius + radius * cos(phi + phi_shift))), (sin(theta + theta_shift) * (outerRadius + radius * cos(phi + phi_shift))), (radius * sin(phi + phi_shift)));

            Vertex v4((cos(theta + theta_shift) * (outerRadius + radius * cos(phi + phi_shift))), (sin(theta + theta_shift) * (outerRadius + radius * cos(phi + phi_shift))), (radius * sin(phi + phi_shift)));
            Vertex v5((cos(theta) * (outerRadius + radius * cos(phi + phi_shift))), (sin(theta) * (outerRadius + radius * cos(phi + phi_shift))), (radius * sin(phi + phi_shift)));
            Vertex v6((cos(theta) * (outerRadius + radius * cos(phi))), (sin(theta) * (outerRadius + radius * cos(phi))), (radius * sin(phi)));

            Triangle t1(v1, v2, v3);
            Triangle t2(v4, v5, v6);
            triangles.push_back(t1);
            triangles.push_back(t2);
            phi = phi_shift * (j + 1);
        }
        theta = theta_shift * (i + 1);
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

Vertex multMatrixVector(vector<Vertex> m, float* v) {

    float x_final = 0.0f;
    float y_final = 0.0f;
    float z_final = 0.0f;

    for (int j = 0; j < 4; ++j) {
        x_final += v[j] * m[j].x;
        y_final += v[j] * m[j].y;
        z_final += v[j] * m[j].z;
    }

    return Vertex(x_final, y_final, z_final);
}

tuple<Vertex,Vertex> bezierCurve(float t, Vertex p1, Vertex p2, Vertex p3, Vertex p4) {
    vector<Vertex> points = { p1,p2,p3,p4 };
    float vectorT[4] = { powf((1 - t),3) , 3 * t * powf((1 - t),2) , 3 * (1 - t) * powf(t,2) , powf(t,3) };

    float vectorDt[4] = {-3 * powf(t,2) + 6 * t -3,9 * powf(t,2) - 12 * t + 3, - 9 * powf(t,2) + 6 * t, 3 * powf(t,2)};

    Vertex resultD = multMatrixVector(points, vectorDt);

    Vertex result = multMatrixVector(points, vectorT);

    return make_tuple(result,resultD);
}

tuple<Vertex,Vertex> bezier(float p, float q, vector<vector<float>> coordinatesVector, vector<int> indexes) {
    vector<Vertex> controlPoints;
    for (int i = 0; i < 4; i++) {

        Vertex p1(coordinatesVector[indexes[4 * i]][0], coordinatesVector[indexes[4 * i]][1], coordinatesVector[indexes[4 * i]][2]);
        Vertex p2(coordinatesVector[indexes[4 * i + 1]][0], coordinatesVector[indexes[4 * i + 1]][1], coordinatesVector[indexes[4 * i + 1]][2]);
        Vertex p3(coordinatesVector[indexes[4 * i + 2]][0], coordinatesVector[indexes[4 * i + 2]][1], coordinatesVector[indexes[4 * i + 2]][2]);
        Vertex p4(coordinatesVector[indexes[4 * i + 3]][0], coordinatesVector[indexes[4 * i + 3]][1], coordinatesVector[indexes[4 * i + 3]][2]);

        Vertex controlPoint = get<0>(bezierCurve(p, p1, p2, p3, p4));

        controlPoints.push_back(controlPoint);

    }
    return bezierCurve(q, controlPoints[0], controlPoints[1], controlPoints[2], controlPoints[3]);
}


void writeFinalPoints(int npatches, int tesselationlevel, vector<vector<int>> indexVector, vector<vector<float>> coordinatesVector, char* fileDest) {
    float step = 1.0f / tesselationlevel;
    float p, p1, q, q1;
    vector<Triangle> triangles;
    vector<Triangle> normals;

    // iteraÃ§Ã£o pelo nÃºmero de patches
    for (int i = 0; i < npatches; i++) {
        for (int line = 0; line < tesselationlevel; line++) {
            for (int column = 0; column < tesselationlevel; column++) {
                p = step * line;
                p1 = step * (line + 1);
                q = step * column;
                q1 = step * (column + 1);

                tuple<Vertex,Vertex> tuple1 = bezier(p, q, coordinatesVector, indexVector[i]);
                Vertex v1 = get<0>(tuple1);
                Vertex n1 = get<1>(tuple1);
                normalize(n1);


                tuple<Vertex,Vertex> tuple2 = bezier(p, q1, coordinatesVector, indexVector[i]);
                Vertex v2 = get<0>(tuple2);
                Vertex n2 = get<1>(tuple2);
                normalize(n2);

                tuple<Vertex,Vertex> tuple3 = bezier(p1, q, coordinatesVector, indexVector[i]);
                Vertex v3 = get<0>(tuple3);
                Vertex n3 = get<1>(tuple3);
                normalize(n3);

                tuple<Vertex,Vertex> tuple4 = bezier(p1, q1, coordinatesVector, indexVector[i]);
                Vertex v4 = get<0>(tuple4);
                Vertex n4 = get<1>(tuple4);
                normalize(n4);

                Triangle tn1(n3,n1,n2);
                Triangle tn2(n2,n4,n1);
                normals.push_back(tn1);
                normals.push_back(tn2);

                Triangle t1(v3, v1, v2);
                Triangle t2(v2, v4, v1);
                triangles.push_back(t1);
                triangles.push_back(t2);

            }
        }
    }
    string path = "../engine/draws/" + (string)fileDest;
    trianglesToFile(triangles, path);
    normalsToFile(normals, path);
}


void readBezier(char* file, int tessellationlevel, char* fileDest) {
    string line;
    int nPatches = 0;
    vector<vector<int>> indexVector;
    int nPoints = 0;

    ifstream myfile;
    myfile.open(file);

    if (myfile.is_open()) {
        // leitura de nÃºmero de patches
        getline(myfile, line);
        nPatches = atoi(line.c_str());

        // leitura dos Ã­ndices
        int x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16;

        for (int i = 0; i < nPatches; i++) {
            getline(myfile, line);
            removeChar(line, ',');
            istringstream data(line.c_str());
            data >> x1 >> x2 >> x3 >> x4 >> x5 >> x6 >> x7 >> x8 >> x9 >> x10 >> x11 >> x12 >> x13 >> x14 >> x15 >> x16;
            vector<int> v = { x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16 };
            indexVector.push_back(v);
        }

        //leitura do nÃºmero de pontos
        getline(myfile, line);
        nPoints = atoi(line.c_str());

        vector<vector<float>> coordinatesVector;

        // leitura das coordenadas dos pontos
        float p1, p2, p3;

        for (int i = 0; i < nPoints; i++) {
            getline(myfile, line);
            removeChar(line, ',');
            istringstream data(line.c_str());
            data >> p1 >> p2 >> p3;
            vector<float> v = { p1,p2,p3 };
            coordinatesVector.push_back(v);
        }
        myfile.close();
        writeFinalPoints(nPatches, tessellationlevel, indexVector, coordinatesVector,fileDest);

    }
}


int main(int argc, char* argv[]) {

    if ((strcmp(argv[1], "plane") == 0) && argc == 4) {
        drawPlane(atof(argv[2]), argv[3]);
        //updateXML(argv[3]);

    }else if ((strcmp(argv[1], "box") == 0) && argc == 6) {
        drawBox(atof(argv[2]), atof(argv[3]), atof(argv[4]), 0 ,argv[5]);
        //updateXML(argv[5]);
    }
    else if ((strcmp(argv[1], "box") == 0) && argc == 7) {
        drawBox(atof(argv[2]), atof(argv[3]), atof(argv[4]), atoi(argv[5]) , argv[6]);
        //updateXML(argv[6]);
    }
    else if ((strcmp(argv[1], "cone") == 0) && argc == 7) {
        drawCone(atof(argv[2]), atof(argv[3]), atoi(argv[4]), atoi(argv[5]), argv[6]);
        //updateXML(argv[6]);
    }
    else if ((strcmp(argv[1], "sphere") == 0) && argc == 6) {
        drawSphere(atof(argv[2]), atoi(argv[3]), atoi(argv[4]), argv[5]);
        //updateXML(argv[5]);
    }
    else if ((strcmp(argv[1], "bezier") == 0) && argc == 5) {
        readBezier(argv[2],atoi(argv[3]),argv[4]);
    }
    else if ((strcmp(argv[1], "torus") == 0) && argc == 7) {
        drawTorus(atof(argv[2]), atof(argv[3]), atof(argv[4]), atof(argv[5]), argv[6]);
    }
    return 0;
}

//	./generator plane 10 plane.3d

// ./generator box x y z size? box.3d

// ./generator cone bottom_radius height slices stacks cone.3d

// ./generator sphere 1 10 10 sphere.3d

// ./generator bezier "file" tessellation "dest.file"

// ./generator torus outerRadius innerRadius slices stacks "torus.3d"