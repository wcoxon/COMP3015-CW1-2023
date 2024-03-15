#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

using glm::vec3;

class Camera{
public:
    float far{ 100 };
    glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3.0f, 1.0f, far);
    glm::mat4 view{ glm::mat4(1.0) };


    void translate(vec3 displacement) {
        view = glm::translate(view, displacement);
    }
    void rotate(float angle, vec3 axis) {
        view = glm::rotate(view, angle, axis);
    }

};