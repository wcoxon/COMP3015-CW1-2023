#pragma once

#include <iostream>
#include <string>
#include "glm/glm.hpp"
#include "helper/scene.h"
#include <vector>
#include <fstream>
#include <glad/glad.h>
#include "helper/glslprogram.h"

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


    //std::vector<Texture> textures;

    Texture colourTexture{ Texture(glm::vec3(1.0f)) };
    Texture normalMap{ Texture(glm::vec3(0.5,0.5,1.)) };

    int verticesCount;
    int indicesCount;

    glm::mat4 transform = glm::mat4(1.0);

    Material mtl;

    Model();

    void loadBufferData(std::vector<glm::vec3> positionData, std::vector<glm::vec3> normalsData, std::vector<glm::vec2> textureData, std::vector<GLuint> indices);

    void loadFileModel(std::string filePath);

    void drawModel();

};
