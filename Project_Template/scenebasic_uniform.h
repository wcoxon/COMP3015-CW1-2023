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
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Mesh.h"

using std::vector;
using glm::vec3;
using glm::mat4;


class SceneBasic_Uniform : public Scene
{
private:
    GLSLProgram prog, shadowProg, directionalShadowProg, waterProg, skyboxProg, postProg, waterDirectionalShadowPass, waterPointShadowPass;

    const unsigned int POINT_SHADOW_RESOLUTION=256;
    const unsigned int DIRECTIONAL_SHADOW_RESOLUTION = 512;


    GLuint renderTexture;
    GLuint renderDepth;
    GLuint renderFBO;
    GLuint depthTex;

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
