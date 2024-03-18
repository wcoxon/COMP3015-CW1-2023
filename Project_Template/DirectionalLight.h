#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
using glm::vec3;
using glm::mat4;

class DirectionalLight{
public:

    vec3 colour{ vec3(1) };
    float intensity{ .7 };

    float far{ 100 };
    float span{ 40 };
    mat4 projection{ glm::ortho(-span, span, -span, span, 0.f, far) };
    mat4 view{ mat4(1.0) };
};