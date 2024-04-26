#pragma once

#include "helper/glslprogram.h"
using glm::vec3;

class Texture
{
public:
    GLuint handle;
    std::string name;

    Texture(vec3 defaultColour = vec3(1, 0, 1));
    void load(std::string filePath);
};