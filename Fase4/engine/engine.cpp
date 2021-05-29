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
#include </usr/local/Cellar/devil/1.8.0_2/include/IL/il.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#include </usr/local/Cellar/devil/1.8.0_2/include/IL/il.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif


#define _USE_MATH_DEFINES
#include <math.h>

int imageWidth;
unsigned char* imageData;

using namespace tinyxml2;
char* pathToXML = (char*)"../engine/cenarios/cenario.xml";

float x = 0.0f;
float z = 0.0f;

float pX = 1.0;
float pY = 2;
float pZ = 1.0;

float camX = 00, camY = 30, camZ = 40;
int startX, startY, tracking = 0;

int alpha = 0, beta = 45, r = 50;
float radius = 50.0f, ri = 35.0f, rc = 15.0f;

int timebase = 0, frame = 0;
float Y[3] = { 0.0f, 1.0f, 0.0f };
float tesselation = 100;


GLuint points;
GLuint normals;
GLuint textures;

//############################### LIGHT CLASS AND LIGHT LOADING #######################################################################################################//


class Light {
    public:
        char* type;
        float pos[4];
        float spotDir[3];
        float attenuation = 1;
        float angle;

        //POINT LIGHT

        Light(char* t, float x, float y, float z, float att) {
            type = t;
            pos[0] = x;
            pos[1] = y;
            pos[2] = z;
            pos[3] = 1.0;
            attenuation = att;
        }

        //SPOTLIGHT LIGHT
        Light(char* t, float posx, float posy, float posz, float ang, float att, float dirx, float diry, float dirz) {
            type = t;
            pos[0] = posx;
            pos[1] = posy;
            pos[2] = posz;
            pos[3] = 1.0;
            angle = isValid(ang) ? ang : 0;
            attenuation = att;
            spotDir[0] = dirx;
            spotDir[1] = diry;
            spotDir[2] = dirz;
        }

        //DIRECCIONAL LIGHT
        Light(char* t, float posx, float posy, float posz) {
            type = t;
            pos[0] = posx;
            pos[1] = posy;
            pos[2] = posz;
            pos[3] = 0.0;
        }

        bool isValid(float ang) {
            return ((ang >= 0 && ang <= 90) || ang == 180);
        }

        Light(const Light& l1) {
            type = l1.type;
            pos[0] = l1.pos[0];
            pos[1] = l1.pos[1];
            pos[2] = l1.pos[2];
            pos[3] = l1.pos[3];
        }
};

std::vector<Light> lights;

void loadLights() {
    int i = 0;
    GLfloat amb[3] = { 0.001, 0.001, 0.001 };
    GLfloat diff[4] = { 1, 1, 1, 1.0 };

    for (auto light : lights) {

        if (i == 8) {
            std::cout << "Reached max number of lights!\n";
            exit(-1);
        }
        glEnable(GL_LIGHT0 + i);
        glLightfv(GL_LIGHT0 + i, GL_AMBIENT, amb); // luz ambiente
        if (strcmp(light.type, "POINT") == 0) {
            glLightfv(GL_LIGHT0 + i, GL_POSITION, light.pos); // posição da luz
            glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, light.attenuation);
        }
        else if (strcmp(light.type, "DIRECTIONAL") == 0) {
            glLightfv(GL_LIGHT0 + i, GL_POSITION, light.pos);
        }
        else if (strcmp(light.type, "SPOT") == 0) {
            glLightfv(GL_LIGHT0, GL_POSITION, light.pos);
            glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light.spotDir);
            glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, light.angle);       // [0,90] ou 180
        }
        else {

        }
        i++;
    }
}

//################################## TEXTURE LOADING ####################################################################################################//


int loadTexture(std::string textureFile) {

    unsigned int t, tw, th;
    unsigned char* texData;
    unsigned int texture;

    ilInit();
    ilEnable(IL_ORIGIN_SET);
    ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

    ilGenImages(1, &t);
    ilBindImage(t);
    const char* textureNameFile= textureFile.c_str();
    ilLoadImage((ILstring)textureNameFile);
    tw = ilGetInteger(IL_IMAGE_WIDTH);
    th = ilGetInteger(IL_IMAGE_HEIGHT);
    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
    texData = ilGetData();

    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

//################################  CATMULL RELATED  ###################################################################################################//


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

    getCatmullRomPoint(t, (float*)basePoints[indices[0]].data(), (float*)basePoints[indices[1]].data(), (float*)basePoints[indices[2]].data(),
                       (float*)basePoints[indices[3]].data(), pos, deriv);
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

//######################################### CLASS GROUP ############################################################################################//

class Group {

public:
    std::vector<std::vector<std::vector<float>>> translation;
    std::vector<float> rotation;
    std::vector<float> scale;
    std::vector<float> vbo;
    std::vector<std::vector<float>> triangleCoordinates;
    std::vector<float> time = { 1,1,1,1,1 };
    std::vector<float> elapsedTime = { 0,0,0,0 };
    std::vector<float> vbo_normals;
    std::vector<float> vbo_texture;
    std::vector<float> rgbDif;
    std::vector<float> rgbSpe;
    std::vector<float> rgbEme;
    std::vector<float> rgbAmb;
    int pos;
    float rotationTime;
    float angle = 0.0f;
    GLuint textureID = 0;

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
        while (getline(File, myText) && (myText.compare("Normals") != 0)) {
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

        while(getline(File, myText) && (myText.compare("Textures") != 0)){

            std::string token = myText.substr(myText.find(delimiter) + 1, myText.length());

            token.erase(std::remove(token.begin(), token.end(), ','), token.end());

            std::istringstream data(token);

            data >> x1 >> x2 >> x3 >> x4 >> x5 >> x6 >> x7 >> x8 >> x9;

            vbo_normals.push_back(x1);
            vbo_normals.push_back(x2);
            vbo_normals.push_back(x3);

            vbo_normals.push_back(x4);
            vbo_normals.push_back(x5);
            vbo_normals.push_back(x6);

            vbo_normals.push_back(x7);
            vbo_normals.push_back(x8);
            vbo_normals.push_back(x9);

        }

        while (getline(File, myText)) {

            std::string token = myText.substr(myText.find(delimiter) + 1, myText.length());

            token.erase(std::remove(token.begin(), token.end(), ','), token.end());

            std::istringstream data(token);

            data >> x1 >> x2 >> x3 >> x4 >> x5 >> x6 >> x7 >> x8 >> x9;

            vbo_texture.push_back(x1);
            vbo_texture.push_back(x2);

            vbo_texture.push_back(x4);
            vbo_texture.push_back(x5);

            vbo_texture.push_back(x7);
            vbo_texture.push_back(x8);
        }
    }
};

//############################## CLASS SCENE ########################################################################################################//


class Scene {
public:
    std::vector<std::string> files;
    std::unordered_map<std::string, std::vector<Group*>> data;

    void addFile(std::string filename, std::vector<std::vector<std::vector<float>>> trans, std::vector<float> rot, std::vector<float> scal,
                                        std::vector<float> t,int pos,float rotT,std::string texture,
                                        std::vector<float> rgbD, std::vector<float> rgbS, std::vector<float> rgbE, std::vector<float> rgbA) {
        Group* g = new Group();
        g->translation = trans;
        g->setRotation(rot[0], rot[1], rot[2]);
        g->setScale(scal[0],scal[1],scal[2]);
        g->read3Dfile(filename);
        g->time = t;
        g->setPos(pos);
        g->rotationTime = rotT;
        g->rgbDif = rgbD;
        g->rgbEme = rgbE;
        g->rgbSpe = rgbS;
        g->rgbAmb = rgbA;

        if (texture.size() > 0) {
            std::string pathText = "../engine/textures/";
            g->textureID = loadTexture(pathText + texture);
        }
        
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

    void readFromXmlRec(XMLElement* element , std::vector<std::vector<std::vector<float>>> trans, std::vector<float> rot,
                        std::vector<float> scal, std::vector<float> time,int pos,float rotT) {
        for (XMLElement* next = element; next != NULL; next = next->NextSiblingElement()) {

            if (strcmp(next->Name(), "rotate") == 0) {
                if (next->FindAttribute("time")) {
                    rotT = atof(next->FindAttribute("time")->Value());
                }

                if (next->FindAttribute("X")) {
                    rot[0] = atof(next->FindAttribute("X")->Value());
                }
                if (next->FindAttribute("Y")) {
                    rot[1] = atof(next->FindAttribute("Y")->Value());
                }
                if (next->FindAttribute("Z")) {
                    rot[2] = atof(next->FindAttribute("Z")->Value());
                }
            }

            else if (strcmp(next->Name(), "translate") == 0) {

                time[pos+1] = atof(next->FindAttribute("time")->Value()) > 0 ? atof(next->FindAttribute("time")->Value()) : 1000;

                std::vector<std::vector<float>> tstate;
                std::vector<float> points;

                for (auto tag = next->FirstChildElement(); tag != NULL; tag = tag->NextSiblingElement()) {

                    std::vector<float> points = { tag->FindAttribute("X") ? (float)atof(tag->FindAttribute("X")->Value()) : 0,
                                                  tag->FindAttribute("Y") ? (float)atof(tag->FindAttribute("Y")->Value()) : 0,
                                                  tag->FindAttribute("Z") ? (float)atof(tag->FindAttribute("Z")->Value()) : 0 };

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
                    std::string textureName = "";
                    std::vector<float> rgbDif = { 0.8, 0.8, 0.8, 1.0 };
                    std::vector<float> rgbEme = { 0, 0, 0, 1 };
                    std::vector<float> rgbSpe = { 0.5, 0.5, 0.5, 1 };
                    std::vector<float> rgbAmb = { 0.5, 0.5, 0.5, 1.0 };

                    if (model->FindAttribute("texture")) {
                        textureName = model->Attribute("texture");
                    }
                    if (model->FindAttribute("emR") && model->FindAttribute("emG") && model->FindAttribute("emB")) {
                        rgbEme = { (float)atof(model->Attribute("emR")),  (float)atof(model->Attribute("emG")) , (float)atof(model->Attribute("emB")) , 1 };
                    }
                    if (model->FindAttribute("difR") && model->FindAttribute("difG") && model->FindAttribute("difB")) {
                        rgbDif = { (float)atof(model->Attribute("difR")),  (float)atof(model->Attribute("difG")) , (float)atof(model->Attribute("difB")) , 1.0 };
                    }
                    if (model->FindAttribute("speR") && model->FindAttribute("speG") && model->FindAttribute("speB")) {
                        rgbSpe = { (float)atof(model->Attribute("speR")),  (float)atof(model->Attribute("speG")) , (float)atof(model->Attribute("speB")), 1.0 };
                    }
                    if (model->FindAttribute("ambR") && model->FindAttribute("ambG") && model->FindAttribute("ambB")) {
                        rgbAmb = { (float)atof(model->Attribute("ambR")),  (float)atof(model->Attribute("ambG")) , (float)atof(model->Attribute("ambB")), 1.0 };
                    }
                    files.push_back(filename);
                    addFile(filename, trans, rot, scal, time, pos, rotT, textureName, rgbDif, rgbSpe, rgbEme, rgbAmb);
                }
            }

            else if (strcmp(next->Name(), "group") == 0) {
                readFromXmlRec(next->FirstChildElement(), trans, rot, scal, time, pos, rotT);
            }

            else {
                std::cout << "Comando Desconhecido" << std::endl;
            }
        }
    }

    void readFromXml() {
        XMLDocument file;
        file.LoadFile(pathToXML);
        // verifica se existe ficheiro ou nï¿½o
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
            int pos = -1;
            float rotT = 1;
            if (strcmp(next->Name(), "lights") == 0) {

                for (auto light = next->FirstChildElement(); light != NULL; light = light->NextSiblingElement()) {
                    if (light->FindAttribute("type")) {
                        //POINT LIGHT
                        if ((strcmp(light->Attribute("type"), "POINT") == 0) && light->FindAttribute("posX") && light->FindAttribute("posY") && light->FindAttribute("posZ") && light->FindAttribute("att")) {
                            Light pointLight("POINT", (float)atof(light->FindAttribute("posX")->Value()), (float)atof(light->FindAttribute("posY")->Value()), (float)atof(light->FindAttribute("posZ")->Value()), (float)atof(light->FindAttribute("att")->Value()));
                            lights.push_back(pointLight);
                        }

                        //SPOTLIGHT LIGHT
                        else if ((strcmp(light->Attribute("type"), "SPOT") == 0) && light->FindAttribute("posX") && light->FindAttribute("posY") && light->FindAttribute("posZ") && light->FindAttribute("ang") && light->FindAttribute("att") && light->FindAttribute("dirX") && light->FindAttribute("dirY") && light->FindAttribute("dirZ")) {
                            Light spotLight("SPOT", (float)atof(light->FindAttribute("posX")->Value()), (float)atof(light->FindAttribute("posY")->Value()), (float)atof(light->FindAttribute("posZ")->Value()), (float)atof(light->FindAttribute("ang")->Value()), (float)atof(light->FindAttribute("att")->Value()), (float)atof(light->FindAttribute("dirX")->Value()), (float)atof(light->FindAttribute("dirY")->Value()), (float)atof(light->FindAttribute("dirZ")->Value()));
                            lights.push_back(spotLight);
                        }

                        //DIRECTIONAL LIGHT
                        else if ((strcmp(light->Attribute("type"), "DIRECTIONAL") == 0) && light->FindAttribute("posX") && light->FindAttribute("posY") && light->FindAttribute("posZ")) {
                            Light dirLight("DIRECTIONAL", (float)atof(light->FindAttribute("posX")->Value()), (float)atof(light->FindAttribute("posY")->Value()), (float)atof(light->FindAttribute("posZ")->Value()));
                            lights.push_back(dirLight);
                        }

                        else {
                            std::cout << "Coordinates on the light tags are not right!\n";
                            exit(-1);
                        }

                    }
                    else {
                        std::cout << "One light source does not have a type!\n";
                        exit(-1);
                    }
                }
            }
            else {
                readFromXmlRec(next, trans, rot, scal, time, pos, rotT);
            }
        }
    }



    void draw() {
        float position[3], derivative[3] = { 0.0f,0.0f,0.0f };
        float M[16];
        float X[3], Z[3];
        for (auto itr = data.begin(); itr != data.end(); itr++) {
            std::string key = itr->first;
            for (auto aux : data[key]) {
                //glColor3f(aux->colors[0]/255, aux->colors[1]/255, aux->colors[2]/255);
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

                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, aux->rgbDif.data());

                glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, aux->rgbSpe.data());

                glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, aux->rgbEme.data());

                glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, aux->rgbAmb.data());

                glBindBuffer(GL_ARRAY_BUFFER, points);
                glBufferData(GL_ARRAY_BUFFER, sizeof(float) * aux->vbo.size(), aux->vbo.data(), GL_STATIC_DRAW);
                glVertexPointer(3, GL_FLOAT, 0, 0);

                if (aux->vbo_normals.size() > 0) {
                    glBindBuffer(GL_ARRAY_BUFFER, normals);
                    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * aux->vbo_normals.size(), aux->vbo_normals.data(), GL_STATIC_DRAW);
                    glNormalPointer(GL_FLOAT, 0, 0);
                }

                if (aux->vbo_texture.size() > 0 && aux->textureID > 0) {
                    glBindBuffer(GL_ARRAY_BUFFER, textures);
                    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * aux->vbo_texture.size(), aux->vbo_texture.data(), GL_STATIC_DRAW);
                    glTexCoordPointer(2, GL_FLOAT, 0, 0);
                    glBindTexture(GL_TEXTURE_2D, aux->textureID);
                }
                
                glDrawArrays(GL_TRIANGLES, 0, ((GLuint)aux->vbo.size() / 3));

                glPopMatrix();

            }
        }
    }
};


//############################# END OF CLASS SCENE #########################################################################################################//


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
    gluLookAt(pX, pY, pZ,
        pX + sin(alpha * M_PI / 180), pY, pZ + cos(alpha * M_PI / 180),
        0.0f, 1.0f, 0.0f);
    //gluLookAt((float)r * sin(alpha) * cos(beta),(float)r * sin(beta), (float)r * cos(alpha) * cos(beta),
    //          0.0f, 0.0f, 0.0f,
    //          0.0f, 1.0f, 0.0f);

// put the geometric transformations here

// put drawing instructions here
    loadLights();
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
            pX += sin(alpha * M_PI / 180);
            pZ += cos(alpha * M_PI / 180);
            break;
        case 'S':
            pX -= sin(alpha * M_PI / 180);
            pZ -= cos(alpha * M_PI / 180);
            break;
        case 'A':
            pX += sin((M_PI / 2) + alpha * M_PI / 180);
            pZ += cos((M_PI / 2) + alpha * M_PI / 180);
            break;
        case 'D':
            pX -= sin((M_PI / 2) + alpha * M_PI / 180);
            pZ -= cos((M_PI / 2) + alpha * M_PI / 180);
            break;
        case 'Z':
            pY += 5.0f;
            break;
        case 'X':
            pY -= 5.0f;
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

void processSpecialKeys(int key, int xx, int yy) {
    if (key == GLUT_KEY_RIGHT) {
        alpha -= 1;
    }
    else if (key == GLUT_KEY_LEFT) {
        alpha += 1;
    }
}

void init(){
    scene->readFromXml();

    // OpenGL settings
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);

    glGenBuffers(1, &points);
    glGenBuffers(1, &normals);
    glGenBuffers(1, &textures);
}


int main(int argc, char **argv) {

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
    glutSpecialFunc(processSpecialKeys);

    #ifndef __APPLE__
        // init GLEW
        glewInit();
    #endif

    init();

// enter GLUT's main cycle
    glutMainLoop();

    return 1;
}