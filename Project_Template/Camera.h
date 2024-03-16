#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

using glm::vec3;

class Camera{
public:

    static Camera* main;


    float far{ 500 };
    vec3 position{vec3(0,20,20)};
    vec3 direction;

    glm::mat4 view{ glm::mat4(1.0) };
    glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3.0f, 1.0f, far);

    void setActive() {
        main = this;
    }

    void updateMatrix() {
        view = glm::lookAt(position, position + direction, vec3(0, 1, 0));
    }

    void translate(vec3 displacement) {
        view = glm::translate(view, displacement);
    }
    void rotate(float angle, vec3 axis) {
        view = glm::rotate(view, angle, axis);
    }

};