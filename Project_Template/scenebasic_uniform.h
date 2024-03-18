#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"
#include <vector>
#include <glad/glad.h>
#include "helper/glslprogram.h"

#include <GLFW/glfw3.h>
#include "helper/glslprogram.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Camera.h"
#include "Mesh.h"

using std::vector;
using glm::vec3;
using glm::mat4;

struct DirectionalLight {

    vec3 colour{ vec3(1) };
    float intensity{ .7};

    float far{ 100 };
    float span{ 40 };
    mat4 projection{ glm::ortho(-span, span, -span, span, 0.f, far) };
    mat4 view{ mat4(1.0) };
};

struct PointLight {
    float intensity{ 10 };
    float far{ 50 };

    vec3 colour{ vec3(1) };

    mat4 projection = glm::perspective(glm::radians(90.0f), 1.f, 0.f, far);
    mat4 transform{ mat4(1.0) };

    PointLight() {}
};

class SceneBasic_Uniform : public Scene
{
private:
    GLSLProgram prog, shadowProg, directionalShadowProg, waterProg, skyboxProg, waterDirectionalShadowPass, waterPointShadowPass;

    vector<PointLight*> pointLights;
    vector<DirectionalLight*> directionalLights;
    

    const unsigned int POINT_SHADOW_RESOLUTION=256;
    const unsigned int DIRECTIONAL_SHADOW_RESOLUTION = 512;

    GLuint directionalTexArray;
    GLuint dirShadowFBO;

    GLuint texCubeArray;
    GLuint shadowFBO;
    
    float time;

    void compile();

public:
    //Camera sceneCamera;
    //vector<Model*> sceneModels;

    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
