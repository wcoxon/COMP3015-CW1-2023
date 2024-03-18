#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
using glm::vec3;
using glm::mat4;

class PointLight{
public:
    float intensity{ 10 };
    float far{ 50 };

    vec3 colour{ vec3(1) };

    mat4 projection = glm::perspective(glm::radians(90.0f), 1.f, 0.f, far);
    mat4 transform{ mat4(1.0) };

    PointLight() {}
};

