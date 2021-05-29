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

void normalize(float* a) {

    float l = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
    a[0] = a[0] / l;
    a[1] = a[1] / l;
    a[2] = a[2] / l;
}

void cross(float* a, float* b, float* res) {

    res[0] = a[1] * b[2] - a[2] * b[1];
    res[1] = a[2] * b[0] - a[0] * b[2];
    res[2] = a[0] * b[1] - a[1] * b[0];
}


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

class Triangle {

public:
    tuple <Vertex, Vertex, Vertex> triangle;

    Triangle() {
        Vertex p(0.0f, 0.0f, 0.0f);

        triangle = make_tuple(p, p, p);
    }

    Triangle(Vertex a, Vertex b, Vertex c) {
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
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

void trianglesToFile(vector<Triangle> t, string f) {
    ofstream file(f);
    unsigned int i = 0;
    int size = t.size() * 3;
    while (i < t.size()) {
        file << "Triangle " << i << ":" << " " << get<0>(t[i].triangle).toString() << " , " << get<1>(t[i].triangle).toString() << " , " << get<2>(t[i].triangle).toString() << endl;
        i++;
    }
}

void normalsToFile(vector<Triangle> normals, string f) {
    ofstream file(f, ofstream::app);
    unsigned int i = 0;
    int size = normals.size() * 3;
    file << "Normals" << endl;
    while (i < normals.size()) {
        file << "Normal " << i << ":" << " " << get<0>(normals[i].triangle).toString() << " , " << get<1>(normals[i].triangle).toString() << " , " << get<2>(normals[i].triangle).toString() << endl;
        i++;
    }
}


void textCoordsToFile(vector<Triangle> textCoords, string f) {
    ofstream file(f, ofstream::app);
    unsigned int i = 0;
    int size = textCoords.size() * 3;
    file << "Textures" << endl;
    while (i < textCoords.size()) {
        file << "TextCoord " << i << ":" << " " << get<0>(textCoords[i].triangle).toString() << " , " << get<1>(textCoords[i].triangle).toString() << " , " << get<2>(textCoords[i].triangle).toString() << endl;
        i++;
    }
}


void drawPlane(float size, string name) {

    float half = size / 2;

    //normais

    Vertex norm1(0.0f, 1.0f, 0.0f);
    Vertex norm2(0.0f, -1.0f, 0.0f);

    //texturas

    Vertex textV1(0, 1, 0);
    Vertex textV2(0, 0, 0);
    Vertex textV3(1, 0, 0);
    Vertex textV4(1, 1, 0);

    Triangle n1(norm1, norm1, norm1);
    Triangle n2(norm2, norm2, norm2);

    //vertices

    Vertex p1(half, 0.0f, half);
    Vertex p2(half, 0.0f, -half);
    Vertex p3(-half, 0.0f, half);
    Vertex p4(-half, 0.0f, -half);

    Triangle t1(p3, p4, p2);
    Triangle t2(p3, p2, p1);
    Triangle t3(p2, p4, p3);
    Triangle t4(p1, p2, p3);


    Triangle text1(textV1, textV2, textV3);
    Triangle text2(textV1, textV3, textV4);
    Triangle text3(textV3, textV2, textV1);
    Triangle text4(textV4, textV3, textV1);


    vector<Triangle> v = { t1,t2,t3,t4 };
    vector<Triangle> n = { n1,n1,n2,n2 };
    vector<Triangle> t = { text1,text2,text3,text4 };

    string path = "../engine/draws/" + name;
    trianglesToFile(v, path);
    normalsToFile(n, path);
    textCoordsToFile(t, path);
}

void drawBox(float x, float y, float z, int w, string name) {

    float newx = x / (w + 1);
    float newz = z / (w + 1);
    float newy = y / (w + 1);

    vector<Triangle> triangles;
    vector<Triangle> normals;
    vector<Triangle> textures;

    for (float i = -(x / 2); i < x / 2; i += newx) {
        for (float j = -(z / 2); j < z / 2; j += newz) {
            // BASE

            //normais

            Vertex norm1(0.0f, -1.0f, 0.0f);
            Triangle n1{ norm1,norm1,norm1 };

            //vertices

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

            //texturas

            Vertex textP1(i / x, j / z, 0);
            Vertex textP2((i + newx) / x, (j / z), 0);
            Vertex textP3((i + newx) / x, (j + newz) / z, 0);
            Vertex textP4(i / x, (j + newz) / z, 0);

            Triangle textureBase1(textP1, textP2, textP3);
            Triangle textureBase2(textP3, textP4, textP1);

            textures.push_back(textureBase1);
            textures.push_back(textureBase2);

            // TOPO

            //normais

            Vertex norm2(0.0f, 1.0f, 0.0f);
            Triangle n2(norm2, norm2, norm2);

            //vertices

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

            //texturas

            Vertex textP5(i / x, j / z, 0);
            Vertex textP6((i + newx) / x, (j + newz) / z, 0);
            Vertex textP7((i + newx) / x, j / z, 0);
            Vertex textP8(i / x, (j + newz) / z, 0);

            Triangle textureTop1(textP5, textP6, textP7);
            Triangle textureTop2(textP5, textP8, textP6);

            textures.push_back(textureTop1);
            textures.push_back(textureTop2);
        }
    }

    for (float i = -(x / 2); i < x / 2; i += newx) {
        for (float j = 0; j < y; j += newy) {
            //FRENTE

            //normais

            Vertex norm3(0.0f, 0.0f, 1.0f);
            Triangle n3{ norm3,norm3,norm3 };

            //vertices

            Vertex p1(i, j, z / 2);
            Vertex p2(i + newx, j, z / 2);
            Vertex p3(i + newx, j + newy, z / 2);
            Vertex p4(i, j + newy, z / 2);

            Triangle t1(p1, p2, p3);
            Triangle t2(p1, p3, p4);

            triangles.push_back(t1);
            triangles.push_back(t2);

            normals.push_back(n3);
            normals.push_back(n3);

            //texturas

            Vertex textP1((i / x), j / y, 0);
            Vertex textP2((i + newx) / x, j / y, 0);
            Vertex textP3((i + newx) / x, (j + newy) / y, 0);
            Vertex textP4((i / x), (j + newy) / y, 0);

            Triangle textureFront1(textP1, textP2, textP3);
            Triangle textureFront2(textP1, textP3, textP4);

            textures.push_back(textureFront1);
            textures.push_back(textureFront2);


            // ATRï¿½S

            //normais

            Vertex norm4(0.0f, 0.0f, -1.0f);
            Triangle n4{ norm4,norm4,norm4 };

            //vertices

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

            //texturas

            Vertex textP5(i / x, j / y, 0);
            Vertex textP6((i + newx) / x, (j + newy) / y, 0);
            Vertex textP7((i + newx) / x, j / y, 0);
            Vertex textP8(i / x, (j + newy) / y, 0);

            Triangle textureBack1(textP5, textP6, textP7);
            Triangle textureBack2(textP5, textP8, textP6);

            textures.push_back(textureBack1);
            textures.push_back(textureBack2);
        }
    }

    for (float i = -(z / 2); i < z / 2; i += newz) {
        for (float j = 0; j < y; j += newy) {
            // FACE DA ESQUERDA

            //normais

            Vertex norm5(-1.0f, 0.0f, 0.0f);
            Triangle n5(norm5, norm5, norm5);

            //vertices

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

            //texturas

            Vertex textP1(j / y, i / z, 0);
            Vertex textP2(j / y, (i + newz) / z, 0);
            Vertex textP3((j + newy) / y, i / z, 0);
            Vertex textP4((j + newy) / y, (i + newz) / z, 0);

            Triangle textureLeft1(textP1, textP2, textP3);
            Triangle textureLeft2(textP2, textP4, textP3);

            textures.push_back(textureLeft1);
            textures.push_back(textureLeft2);

            // FACE DA DIREITA

            //normais

            Vertex norm6(1.0f, 0.0f, 0.0f);
            Triangle n6(norm6, norm6, norm6);

            //vertices

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

            //texturas

            Vertex textP5(j / y, i / z, 0);
            Vertex textP6((j + newy) / y, i / z, 0);
            Vertex textP7(j / y, (i +  newz) / z, 0);
            Vertex textP8((j + newy) / y, (i + newz) / z, 0);

            Triangle textureRight1(textP5, textP6, textP7);
            Triangle textureRight2(textP7, textP6, textP8);

            textures.push_back(textureRight1);
            textures.push_back(textureRight2);
        }
    }

    string path = "../engine/draws/" + name;
    trianglesToFile(triangles, path);
    normalsToFile(normals, path);
    textCoordsToFile(textures, path);
}

void drawSphere(float radius, int slices, int stacks, string name) {
    float slices_alpha = (2 * M_PI) / (float)slices;
    float stacks_beta = (M_PI) / (float)stacks;
    vector<Triangle> triangles;
    vector<Triangle> normals;
    vector<Triangle> textures;

    for (int i = 0; i < slices; i++) {
        float alpha = i * slices_alpha;
        for (int j = 0; j < stacks; j++) {
            float beta = (M_PI / 2) - (j * stacks_beta);
            //vertices
            Vertex p1(radius * cos(beta) * sin(alpha), radius * sin(beta), radius * cos(beta) * cos(alpha));
            Vertex p2(radius * cos(beta) * sin(alpha + slices_alpha), radius * sin(beta), radius * cos(beta) * cos(alpha + slices_alpha));
            Vertex p3(radius * cos(beta + stacks_beta) * sin(alpha + slices_alpha), radius * sin(beta + stacks_beta), radius * cos(beta + stacks_beta) * cos(alpha + slices_alpha));
            Vertex p4(radius * cos(beta + stacks_beta) * sin(alpha), radius * sin(beta + stacks_beta), radius * cos(beta + stacks_beta) * cos(alpha));

            Triangle t1(p1, p2, p3);
            Triangle t2(p1, p3, p4);

            //normais
            Vertex n1(p1.x, p1.y, p1.z);
            Vertex n2(p2.x, p2.y, p2.z);
            Vertex n3(p3.x, p3.y, p3.z);
            Vertex n4(p4.x, p4.y, p4.z );

            normalize(n1);
            normalize(n2);
            normalize(n3);
            normalize(n4);

            Triangle tn1(n1, n2, n3);
            Triangle tn2(n1, n3, n4);

            //texturas
            Vertex textV2(float(i + 1) / float(slices), float(j) / float(stacks), 0);
            Vertex textV4(float(i) / float(slices), float(j + 1) / float(stacks), 0);
            Vertex textV3(float(i + 1) / float(slices), float(j + 1) / float(stacks), 0);
            Vertex textV1(float(i) / float(slices), float(j) / float(stacks), 0);

            Triangle textureT1(textV1, textV2, textV3);
            Triangle textureT2(textV1, textV3, textV4);

            textures.push_back(textureT1);
            textures.push_back(textureT2);

            normals.push_back(tn1);
            normals.push_back(tn2);

            triangles.push_back(t1);
            triangles.push_back(t2);
        }
    }
    string path = "../engine/draws/" + name;
    trianglesToFile(triangles, path);
    normalsToFile(normals, path);
    textCoordsToFile(textures, path);
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

            //vertices

            Vertex p1(bottom_radius * sin(delta_alpha), bottom, bottom_radius * cos(delta_alpha));
            Vertex p2(upper_radius * sin(delta_alpha + alpha), upper, upper_radius * cos(delta_alpha + alpha));
            Vertex p3(upper_radius * sin(delta_alpha), upper, upper_radius * cos(delta_alpha));
            Vertex p4(bottom_radius * sin(delta_alpha + alpha), bottom, bottom_radius * cos(delta_alpha + alpha));

            Triangle t1(p1, p2, p3);
            Triangle t2(p1, p4, p2);

            //normais

            Vertex n1(p1.x, p1.y, p1.z);
            Vertex n2(p2.x, p2.y, p2.z);
            Vertex n3(p3.x, p3.y, p3.z);
            Vertex n4(p4.x, p4.y, p4.z);

            normalize(n1);
            normalize(n2);
            normalize(n3);
            normalize(n4);

            Triangle tn1(n1, n2, n3);
            Triangle tn2(n1, n3, n4);

            normals.push_back(tn1);
            normals.push_back(tn2);

            triangles.push_back(t1);
            triangles.push_back(t2);

        }
    }

    string path = "../engine/draws/" + name;
    trianglesToFile(triangles, path);
    normalsToFile(normals, path);
}


void drawTorus(float outerRadius, float radius, int slices, int stacks, string name) {

    float theta = 0;
    float phi = 0;
    float theta_shift = (2 * M_PI) / slices;
    float phi_shift = (2 * M_PI) / stacks;
    vector<Triangle> triangles;
    vector<Triangle> normals;
    vector<Triangle> textures;

    float textStacks = 1 / stacks;
    float textSlices = 1 / slices;

    for (int i = 0; i < slices; i++) {
        for (int j = 0; j < stacks; j++) {

            //vertices

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

            //normais

            Vertex n1(cos(theta) * cos(phi), sin(phi), sin(theta) * cos(phi) );
            Vertex n2(cos(theta + theta_shift) * cos(phi),  sin(phi), sin(theta + theta_shift) * cos(phi));
            Vertex n3(cos(theta + theta_shift) * cos(phi + phi_shift), sin(phi + phi_shift), sin(theta + theta_shift) * cos(phi + phi_shift));

            Vertex n4(cos(theta + theta_shift) * cos(phi + phi_shift),  sin(phi + phi_shift), sin(theta + theta_shift) * cos(phi + phi_shift));
            Vertex n5(cos(theta) * cos(phi + phi_shift),  sin(phi + phi_shift), sin(theta) * cos(phi + phi_shift));
            Vertex n6(cos(theta) * cos(phi),  sin(phi), sin(theta) * cos(phi));


            Triangle tn1(n1, n2, n3);
            Triangle tn2(n4, n5, n6);

            normals.push_back(tn1);
            normals.push_back(tn2);

            //texturas

            Vertex textP1(j * textStacks, i * textSlices, 0);
            Vertex textP2(j * textStacks, (i + 1) * textSlices, 0);
            Vertex textP3((j + 1) * textStacks, (i + 1) * textSlices, 0);
            Vertex textP4((j + 1) * textStacks, i * textSlices, 0);

            Triangle textureT1(textP1, textP2, textP3);
            Triangle textureT2(textP3, textP4, textP1);

            textures.push_back(textureT1);
            textures.push_back(textureT2);

            phi = phi_shift * (j + 1);
        }
        theta = theta_shift * (i + 1);
    }

    string path = "../engine/draws/" + name;
    trianglesToFile(triangles, path);
    normalsToFile(normals, path);
    textCoordsToFile(textures, path);
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
    Vertex final(x_final, y_final, z_final);
    return final;
}

void multVectorMatrix2(float** m, float* v, float* res) {

    for (int j = 0; j < 4 ; ++j) {
        res[j] = 0;
        for (int k = 0; k < 4; ++k) {
            res[j] += v[k] * m[k][j];
        }
    }
}


Vertex bezierCurve(float t, Vertex p1, Vertex p2, Vertex p3, Vertex p4) {
    vector<Vertex> points = { p1,p2,p3,p4 };
    float vectorT[4] = { powf((1 - t),3) , 3 * t * powf((1 - t),2) , 3 * (1 - t) * powf(t,2) , powf(t,3) };

    Vertex result = multMatrixVector(points, vectorT);


    return result;
}

float** convertVVtoFloat(std::vector<std::vector<float>> matrix) {
    float** res = (float**)malloc(sizeof(float*) * 5);

    for (int efe = 0; efe < 5; efe++) {
        res[efe] = (float*)malloc(sizeof(float) * 5);
    }

    for (std::vector<float>::size_type i = 0; i < matrix.size(); i++) {
        for (std::vector<float>::size_type j = 0; j < matrix[0].size(); j++) {
            res[i][j] = matrix[i][j];
        }
    }
    return res;
}

float** getdVectorsUandV(float u, float v, vector<vector<float>> allPoints, vector<int> index) {
    float resU[3];
    float resV[3];
    float** res = (float**)malloc(sizeof(float*) * 3);

    for (int efe = 0; efe < 3; efe++) {
        res[efe] = (float*)malloc(sizeof(float) * 16);
    }

    float **matrixP[3];

    for (int k = 0; k < 3; k++) {
        matrixP[k] = (float**)malloc(sizeof(float*) * 5);

        for (int j = 0; j < 4; j++) {
            matrixP[k][j] = (float*)malloc(sizeof(float) * 5);

        }

    }

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {

            vector<float> p0 = allPoints[index[4*i+j]];
            matrixP[0][i][j] = p0[0];
            matrixP[1][i][j] = p0[1];
            matrixP[2][i][j] = p0[2];

        }

    }
    float vectordU[4] = { 3* powf(u,2), 2 * u , 1 , 0};
    std::vector<std::vector<float>> vectorV = { {powf(v,3)}, {powf(v,2)},{v},{1 }};
    float vectorU[4] = { powf(u,3), powf(u,2) , u , 1 };
    std::vector<std::vector<float>> vectordV = { {3 * powf(v,2) }, { 2 * v }, { 1 }, { 0 } };


    std::vector<std::vector<float>> matrixM;

    matrixM = { {-1.0f,  3.0f, -3.0f,  1.0f},
                { 3.0f, -6.0f,  3.0f,  0.0f},
                {-3.0f,  3.0f,  0.0f,  0.0f},
                { 1.0f,  0.0f,  0.0f,  0.0f} };



    float** mtrixM = convertVVtoFloat(matrixM);

    //vector dU.M [FOR VECTOR U]
    float* dxUM = (float*)malloc(sizeof(float*) * 10);
    multVectorMatrix2(mtrixM, vectordU, dxUM);

    //vector U.M  [FOR VECTOR V]
    float* UxM = (float*)malloc(sizeof(float*) * 10);
    multVectorMatrix2(mtrixM, vectorU, UxM);

    for (int i = 0; i < 3; i++) {

        /*                  CALCULATE VECTOR U                  */
        //vector (dU.M).P
        float* dUxMxP = (float*)malloc(sizeof(float*) * 10);
        multVectorMatrix2(matrixP[i], dxUM, dUxMxP);

        //Vector ((dU.M).P) .Mt
        float* dUxMPxMt = (float*)malloc(sizeof(float*) * 10);
        multVectorMatrix2(mtrixM, dUxMxP, dUxMPxMt);

        resU[i] = dUxMPxMt[0] * vectorV[0][0] + dUxMPxMt[1] * vectorV[1][0] + dUxMPxMt[2] * vectorV[2][0] + dUxMPxMt[3] * vectorV[3][0];
        /*                  ------------------                  */


        /*                  CALCULATE VECTOR V                  */
        //vector (U.M).P
        float* UxMxP = (float*)malloc(sizeof(float*) * 10);
        multVectorMatrix2(matrixP[i], UxM, UxMxP);

        //Vector ((U.M).P) .Mt
        float* UxMPxMt = (float*)malloc(sizeof(float*) * 10);
        multVectorMatrix2(mtrixM, UxMxP, UxMPxMt);

        resV[i] = UxMPxMt[0] * vectordV[0][0] + UxMPxMt[1] * vectordV[1][0] + UxMPxMt[2] * vectordV[2][0] + UxMPxMt[3] * vectordV[3][0];

    }
    //printf("U: %f %f %f \n", resU[0],resU[1],resU[2]);
    //printf("V: %f %f %f \n", resV[0], resV[1], resV[2]);
    //printf("-------\n");
    res[0] = resU;
    res[1] = resV;
    return res;
}

Vertex bezier(float p, float q, vector<vector<float>> coordinatesVector, vector<int> indexes) {
    vector<Vertex> controlPoints;
    vector<Vertex> points;

    for (int i = 0; i < 4; i++) {


        Vertex p1(coordinatesVector[indexes[4 * i]][0], coordinatesVector[indexes[4 * i]][1], coordinatesVector[indexes[4 * i]][2]);
        Vertex p2(coordinatesVector[indexes[4 * i + 1]][0], coordinatesVector[indexes[4 * i + 1]][1], coordinatesVector[indexes[4 * i + 1]][2]);
        Vertex p3(coordinatesVector[indexes[4 * i + 2]][0], coordinatesVector[indexes[4 * i + 2]][1], coordinatesVector[indexes[4 * i + 2]][2]);
        Vertex p4(coordinatesVector[indexes[4 * i + 3]][0], coordinatesVector[indexes[4 * i + 3]][1], coordinatesVector[indexes[4 * i + 3]][2]);

        Vertex controlPoint = bezierCurve(p, p1, p2, p3, p4);

        controlPoints.push_back(controlPoint);
    }

    return bezierCurve(q, controlPoints[0], controlPoints[1], controlPoints[2], controlPoints[3]);
}


void writeFinalPoints(int npatches, int tesselationlevel, vector<vector<int>> index, vector<vector<float>> allPoints, char* fileDest) {
    float step = 1.0f / tesselationlevel;
    float u, u1, v, v1;
    vector<Triangle> triangles;
    vector<Triangle> normals;
    vector<Triangle> textures;

    // iteraÃ§Ã£o pelo nÃºmero de patches
    for (int i = 0; i < npatches; i++) {
        for (int line = 0; line < tesselationlevel; line++) {
            for (int column = 0; column < tesselationlevel; column++) {
                u = step * line;
                u1 = step * (line + 1);
                v = step * column;
                v1 = step * (column + 1);

                Vertex aux = bezier(u, v, allPoints, index[i]);
                Vertex aux1 = bezier(u, v1, allPoints, index[i]);
                Vertex aux2 = bezier(u1, v, allPoints, index[i]);
                Vertex aux3 = bezier(u1, v1, allPoints, index[i]);

                auto vectorsPA = getdVectorsUandV(u, v, allPoints, index[i]);
                auto vectorsPB = getdVectorsUandV(u, v1, allPoints, index[i]);
                auto vectorsPC = getdVectorsUandV(u1, v, allPoints, index[i]);
                auto vectorsPD = getdVectorsUandV(u1, v1, allPoints, index[i]);

                float vectorUpA[3] = { vectorsPA[0][0], vectorsPA[0][1],vectorsPA[0][2] };
                float vectorUpB[3] = { vectorsPB[0][0], vectorsPB[0][1],vectorsPB[0][2] };
                float vectorUpC[3] = { vectorsPC[0][0], vectorsPC[0][1],vectorsPC[0][2] };
                float vectorUpD[3] = { vectorsPD[0][0], vectorsPD[0][1],vectorsPD[0][2] };

                float vectorVpA[3] = { vectorsPA[1][0], vectorsPA[1][1],vectorsPA[1][2] };
                float vectorVpB[3] = { vectorsPB[1][0], vectorsPB[1][1],vectorsPB[1][2] };
                float vectorVpC[3] = { vectorsPC[1][0], vectorsPC[1][1],vectorsPC[1][2] };
                float vectorVpD[3] = { vectorsPD[1][0], vectorsPD[1][1],vectorsPD[1][2] };

                float* auxNormalPA = (float*)malloc(sizeof(float) * 10);
                float* auxNormalPB = (float*)malloc(sizeof(float) * 10);
                float* auxNormalPC = (float*)malloc(sizeof(float) * 10);
                float* auxNormalPD = (float*)malloc(sizeof(float) * 10);

                cross(vectorUpA, vectorVpA, auxNormalPA);
                cross(vectorUpB, vectorVpB, auxNormalPB);
                cross(vectorUpC, vectorVpC, auxNormalPC);
                cross(vectorUpD, vectorVpD, auxNormalPD);

                normalize(auxNormalPA);
                normalize(auxNormalPB);
                normalize(auxNormalPC);
                normalize(auxNormalPD);

                Vertex pA(aux.x, aux.y, aux.z);
                Vertex pB(aux1.x, aux1.y, aux1.z);
                Vertex pC(aux2.x, aux2.y, aux2.z);
                Vertex pD(aux3.x, aux3.y, aux3.z);

                Vertex normalPA(auxNormalPA[0], auxNormalPA[1], auxNormalPA[2]);
                Vertex normalPB(auxNormalPB[0], auxNormalPB[1], auxNormalPB[2]);
                Vertex normalPC(auxNormalPC[0], auxNormalPC[1], auxNormalPC[2]);
                Vertex normalPD(auxNormalPD[0], auxNormalPD[1], auxNormalPD[2]);

                Triangle t1(pC, pA, pB);
                Triangle t2(pB, pD, pC);
                triangles.push_back(t1);
                triangles.push_back(t2);

                Triangle normalT1(normalPC, normalPA, normalPB);
                Triangle normalT2(normalPB, normalPD, normalPC);
                normals.push_back(normalT1);
                normals.push_back(normalT2);

                Vertex textPA(u, v, 0);
                Vertex textPB(u, v1, 0);
                Vertex textPC(u1, v, 0);
                Vertex textPD(u1, v1, 0);

                Triangle text1(textPC, textPA, textPB);
                Triangle text2(textPB, textPD, textPC);
                textures.push_back(text1);
                textures.push_back(text2);

            }
        }
    }
    string path = "../engine/draws/" + (string)fileDest;
    trianglesToFile(triangles, path);
    normalsToFile(normals, path);
    textCoordsToFile(textures,path);
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
        writeFinalPoints(nPatches, tessellationlevel, indexVector, coordinatesVector, fileDest);

    }
}


int main(int argc, char* argv[]) {

    if ((strcmp(argv[1], "plane") == 0) && argc == 4) {
        drawPlane(atof(argv[2]), argv[3]);
    }
    else if ((strcmp(argv[1], "box") == 0) && argc == 6) {
        drawBox(atof(argv[2]), atof(argv[3]), atof(argv[4]), 0, argv[5]);
    }
    else if ((strcmp(argv[1], "box") == 0) && argc == 7) {
        drawBox(atof(argv[2]), atof(argv[3]), atof(argv[4]), atoi(argv[5]), argv[6]);
    }
    else if ((strcmp(argv[1], "cone") == 0) && argc == 7) {
        drawCone(atof(argv[2]), atof(argv[3]), atoi(argv[4]), atoi(argv[5]), argv[6]);
    }
    else if ((strcmp(argv[1], "sphere") == 0) && argc == 6) {
        drawSphere(atof(argv[2]), atoi(argv[3]), atoi(argv[4]), argv[5]);
    }
    else if ((strcmp(argv[1], "bezier") == 0) && argc == 5) {
        readBezier(argv[2], atoi(argv[3]), argv[4]);
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