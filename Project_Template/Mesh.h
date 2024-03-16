#pragma once

#include <iostream>
#include <string>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <vector>
#include <fstream>
#include <glad/glad.h>
#include "helper/glslprogram.h"

using glm::vec3;

struct Texture {
    GLuint handle;
    std::string name;

    Texture(glm::vec3 defaultColour = glm::vec3(0));
    void load(std::string filePath);
};

struct Material {

    float ambientReflectivity{ .1 };
    float diffuseReflectivity{ .5 };
    float specularReflectivity{ .5 };
    int specularPower{ 10 };

    bool shadeFlat = false;
    bool perFragment = true;

};

class Model {
public:
    GLSLProgram* program;

    GLuint vaoHandle;
    GLuint vboHandles[2];

    GLuint drawMode = GL_TRIANGLES;
    //std::vector<Texture> textures;

    Texture colourTexture{ Texture(glm::vec3(1.0f)) };
    Texture normalMap{ Texture(glm::vec3(0.5,0.5,1.)) };

    int verticesCount;
    int indicesCount;

    vec3 scale;
    vec3 position;
    vec3 direction{ vec3(0,0,-1) };

    glm::mat4 transform = glm::mat4(1.0);

    Material mtl;

    Model();

    void loadBufferData(std::vector<glm::vec3> positionData, std::vector<glm::vec3> normalsData, std::vector<glm::vec2> textureData, std::vector<GLuint> indices);

    void loadFileModel(std::string filePath);

    void drawModel();

    void updateMatrix() {
        transform = glm::inverse(glm::lookAt(position, position + direction, vec3(0, 1, 0)));// *glm::scale(glm::mat4(1), scale);//glm::translate(glm::mat4(1), position)*glm::scale(glm::mat4(1),scale);
    }
    

    void rotate(float angle, glm::vec3 axis) {
        transform = glm::rotate(transform, angle, axis);
    }
    void translate(glm::vec3 displacement) {
        position += displacement;
        //transform = glm::translate(transform, displacement);
    }
};
