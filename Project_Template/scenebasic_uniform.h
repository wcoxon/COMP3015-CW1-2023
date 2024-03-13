#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"
#include <vector>
#include <glad/glad.h>
#include "helper/glslprogram.h"
#include "Mesh.h"

#include "helper/glslprogram.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
using std::vector;
using glm::vec3;

struct Camera {
    glm::mat4 projection = glm::perspective(45.0, 4.0 / 3.0, 1.0, 50.0);
    glm::mat4 view{ glm::mat4(1.0) };
};

struct DirectionalLight {

    glm::vec3 colour{ glm::vec3(1) };
    float intensity{ .4 };

    float far{ 60 };
    glm::mat4 projection{ glm::ortho(-20.f, 20.f, -20.f, 20.f, 0.f, far) };
    glm::mat4 view{ glm::mat4(1.0) };
};

struct PointLight {
    float intensity{ 10 };
    float far{ 20 };

    glm::vec3 colour{ glm::vec3(1) };

    glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.f, 0.f, far);
    glm::mat4 transform{ glm::mat4(1.0) };

    PointLight() {}

    /*vector<glm::mat4> getCubemapMatrices() {
        //glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.f, 0.f, far);
        glm::mat4 PV = projection * transform;
        return {
            PV* glm::lookAt(vec3(0), glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
            PV* glm::lookAt(vec3(0), glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
            PV* glm::lookAt(vec3(0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
            PV* glm::lookAt(vec3(0), glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)),
            PV* glm::lookAt(vec3(0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)),
            PV* glm::lookAt(vec3(0), glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0))
        };
    }*/
};

class SceneBasic_Uniform : public Scene
{
private:
    GLSLProgram prog, shadowProg, directionalShadowProg, waterProg, skyboxProg;

    vector<Model*> sceneModels;
    vector<PointLight*> pointLights;
    vector<DirectionalLight*> directionalLights;
    Camera sceneCamera;

    const unsigned int SHADOW_RESOLUTION=256;


    GLuint directionalTexArray;
    GLuint dirShadowFBO;

    GLuint texCubeArray;
    GLuint shadowFBO;
    
    float time;

    void compile();

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
