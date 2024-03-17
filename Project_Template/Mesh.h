#pragma once

#include <iostream>
#include <string>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <vector>
#include <fstream>
#include <glad/glad.h>
#include "helper/glslprogram.h"

using std::vector;
using glm::vec3;

struct Texture {
    GLuint handle;
    std::string name;

    Texture(vec3 defaultColour = vec3(0));
    void load(std::string filePath);
};

struct Material {

    bool shadeFlat = false;

    float ambientReflectivity{ .1 };
    float diffuseReflectivity{ .5 };
    float specularReflectivity{ .5 };
    int specularPower{ 10 };

    bool perFragment = true;

};

class Model {
public:
    GLSLProgram* program;

    GLuint vaoHandle;
    GLuint vboHandles[2];

    GLuint drawMode = GL_TRIANGLES;
    //vector<Texture> textures;

    Texture colourTexture{ Texture(vec3(1.0f)) };
    Texture normalMap{ Texture(vec3(0.5,0.5,1.)) };

    int verticesCount;
    int indicesCount;

    vec3 scale;
    vec3 position;
    vec3 direction{ vec3(0,0,-1) };
    vec3 up{ vec3(0,1,0) };

    glm::mat4 transform = glm::mat4(1.0);

    Material mtl;

    Model();

    void loadBufferData(vector<vec3> positionData, vector<vec3> normalsData, vector<glm::vec2> textureData, vector<GLuint> indices);

    void loadFileModel(std::string filePath);

    void drawModel();


    void setUp(vec3 direction) {
        up = direction;
    }
    void lookAt(vec3 target) {
        direction = glm::normalize(target - position);
    }
    void translate(vec3 displacement) {
        position += displacement;
    }

    void updateMatrix() {
        transform = glm::inverse(glm::lookAt(position, position + direction, up)) *glm::scale(glm::mat4(1), scale);//glm::translate(glm::mat4(1), position)*glm::scale(glm::mat4(1),scale);
    }
};
