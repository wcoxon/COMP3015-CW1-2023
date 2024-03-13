#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>

#include <string>
using std::string;

#include <iostream>
using std::cerr;
using std::cout;
using std::endl;

#include "helper/glutils.h"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include "Mesh.h"


//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


using glm::vec3;

SceneBasic_Uniform::SceneBasic_Uniform() : time(0.0f) {}


unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

Model* skybox;

Model* ball;
Model* table;

void SceneBasic_Uniform::initScene()
{
    glFrontFace(GL_CW);
    glEnable(GL_DEPTH_TEST);
    //glClearColor(0.9f, 0.8f, 1.f, 1.0f);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    compile();

    Model* water = new Model();
    water->loadFileModel("./media/subdivplane.obj");
    water->normalMap.load("./media/textures/0001.png");
    water->transform = glm::translate(glm::mat4(1.0), vec3(0, 0, 0)) * glm::scale(glm::mat4(1.0), glm::vec3(20));
    water->program = &waterProg;
    water->mtl.diffuseReflectivity = 0.1f;
    water->mtl.specularReflectivity = 0.5f;
    water->mtl.specularPower = 32;
    sceneModels.push_back(water);
    
    Model* wall = new Model();
    wall->loadFileModel("./media/wall.obj");
    //wall->colourTexture.load("./media/textures/pebblesD.jpg");
    //wall->normalMap.load("./media/textures/pebblesNormal.jpg");
    wall->program = &prog;
    wall->mtl.specularReflectivity = 0.f;
    wall->transform = glm::translate(glm::mat4(1.0), vec3(-1, -1, -20)) * glm::rotate(glm::mat4(1.0), glm::radians(-90.f), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1.0),vec3(2));
    wall->mtl.shadeFlat = true;
    sceneModels.push_back(wall);
    

    table = new Model();
    table->transform = glm::translate(glm::mat4(1.0), vec3(-1, 0, -5)) * glm::scale(glm::mat4(1.0), vec3(3));
    table->loadFileModel("./media/table.obj");
    table->colourTexture.load("./media/textures/wood.jpg");
    table->program = &prog;
    table->mtl.specularReflectivity = 0.4f;
    table->mtl.diffuseReflectivity = 1.f;
    table->mtl.shadeFlat = true;
    sceneModels.push_back(table);

    ball = new Model();
    ball->loadFileModel("./media/icosphere.obj");
    ball->program = &prog;
    ball->transform =
        glm::translate(glm::mat4(1.0), vec3(-2, 1, 5))*
        glm::scale(glm::mat4(1.0),glm::vec3(1.5));
    sceneModels.push_back(ball);

    //Model* skybox = new Model();
    // 1 - make model without loading from file
    // 2 - dynamic vertex attributes - skybox only needs positions and cubemap uniform
    // 3 - make shaders for this, put quad on far plane and sample view directions on cubemap

    skybox = new Model();
    std::vector<glm::vec3> skyquadVerts = { {-1,-1,-1},{-1,1,-1} ,{1,-1,-1} ,{1,1,-1} };
    //std::vector<glm::vec3> skyquadNormals = { {0,0,1},{0,0,1} ,{0,0,1} ,{0,0,1} };
    //std::vector<glm::vec2> skyquadTex = { {0,0},{0,1} ,{1,0} ,{1,1} };
    std::vector<GLuint> skyquadIndices = { 0,1,2, 1,2,3 };

    skybox->program = &skyboxProg;
    glBindVertexArray(skybox->vaoHandle);
    //
    glBindBuffer(GL_ARRAY_BUFFER, skybox->vboHandles[0]);
    glBufferData(GL_ARRAY_BUFFER, skyquadVerts.size() * sizeof(glm::vec3), skyquadVerts.data(), GL_STATIC_DRAW);
    //
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    //
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skybox->vboHandles[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, skyquadIndices.size() * sizeof(GLuint), skyquadIndices.data(), GL_STATIC_DRAW);
    //
    std::vector<std::string> skyboxTexturePaths = { "G:/skybox/skybox/right.jpg","G:/skybox/skybox/left.jpg","G:/skybox/skybox/top.jpg","G:/skybox/skybox/bottom.jpg","G:/skybox/skybox/front.jpg","G:/skybox/skybox/back.jpg" };
    int skyboxTextureID = loadCubemap(skyboxTexturePaths);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);
    

    glGenFramebuffers(1, &shadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glGenTextures(1, &texCubeArray);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, texCubeArray);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    int numLights = 2;
    glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 1, GL_DEPTH_COMPONENT24, SHADOW_RESOLUTION, SHADOW_RESOLUTION, numLights * 6);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texCubeArray, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, 0);

    glm::mat4 shadowProj = glm::perspective(glm::half_pi<float>(), 1.f, 0.f, 20.0f);

    pointLights.push_back(new PointLight());
    pointLights[0]->transform = glm::translate(glm::mat4(1.0), vec3(-3, -4, -3));
    pointLights[0]->intensity = 15;

    pointLights.push_back(new PointLight());
    pointLights[1]->transform = glm::translate(glm::mat4(1.0),vec3(8,-4,-5));
    pointLights[1]->colour = vec3(0,0.5,1);
    

    glGenFramebuffers(1, &dirShadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, dirShadowFBO);
    glGenTextures(1, &directionalTexArray);
    glBindTexture(GL_TEXTURE_2D_ARRAY, directionalTexArray);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH_COMPONENT24, SHADOW_RESOLUTION, SHADOW_RESOLUTION, 1);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, directionalTexArray, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("Error: framebuffer is not complete!");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    directionalLights.push_back(new DirectionalLight());
    directionalLights[0]->view =glm::lookAt(glm::vec3(-20.f, 20.f, -20.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.0f, 1.0f, 0.0f));


    shadowProg.use();
    shadowProg.printActiveUniforms();

    //set point light uniforms
    shadowProg.setUniform("lights[0].project", shadowProj);
    shadowProg.setUniform("lights[0].transform", pointLights[0]->transform);

    shadowProg.setUniform("lights[1].project", shadowProj);
    shadowProg.setUniform("lights[1].transform", pointLights[1]->transform);

    shadowProg.setUniform("far_plane", 20.0f);


    directionalShadowProg.use();
    directionalShadowProg.printActiveUniforms();

    //set uniforms for directional light
    directionalShadowProg.setUniform("lights[0].transform", directionalLights[0]->view);
    directionalShadowProg.setUniform("lights[0].project", directionalLights[0]->projection);
    directionalShadowProg.setUniform("far_plane", directionalLights[0]->far);




    sceneCamera.view =
        glm::translate(glm::mat4(1.0), vec3(0, -5, -20.0)) *
        glm::rotate(glm::mat4(1.0), glm::radians(30.0f), vec3(1, 0, 0)) *
        glm::rotate(glm::mat4(1.0), glm::radians(-30.f), vec3(0, 1, 0));


    prog.use();
    cout << "standard shaders:" << endl;
    prog.printActiveUniforms();

    prog.setUniform("view", sceneCamera.view);
    prog.setUniform("projection", sceneCamera.projection);

    prog.setUniform("lights[0].lightIntensity", pointLights[0]->intensity);
    prog.setUniform("lights[0].lightColour", pointLights[0]->colour);
    prog.setUniform("lights[0].textureID", 0);
    prog.setUniform("lights[0].transform", pointLights[0]->transform);
    prog.setUniform("lights[0].far_plane", pointLights[0]->far);

    prog.setUniform("lights[1].lightIntensity", pointLights[1]->intensity);
    prog.setUniform("lights[1].lightColour", pointLights[1]->colour);
    prog.setUniform("lights[1].textureID", 1);
    prog.setUniform("lights[1].transform", pointLights[1]->transform);
    prog.setUniform("lights[1].far_plane", pointLights[1]->far);


    prog.setUniform("directionalLights[0].lightIntensity", directionalLights[0]->intensity);
    prog.setUniform("directionalLights[0].lightColour", directionalLights[0]->colour);
    prog.setUniform("directionalLights[0].textureID", 0);
    prog.setUniform("directionalLights[0].project", directionalLights[0]->projection);
    prog.setUniform("directionalLights[0].transform", directionalLights[0]->view);
    prog.setUniform("directionalLights[0].far_plane", directionalLights[0]->far);

    prog.setUniform("colourTexture", 0);
    prog.setUniform("normalMap", 1);
    prog.setUniform("pointDepthMaps", 2);
    prog.setUniform("directionalDepthMaps", 3);


    waterProg.use();
    cout << "water shaders:" << endl;
    waterProg.printActiveUniforms();

    waterProg.setUniform("view", sceneCamera.view);
    waterProg.setUniform("projection", sceneCamera.projection);

    waterProg.setUniform("lights[0].lightIntensity", pointLights[0]->intensity);
    waterProg.setUniform("lights[0].lightColour", pointLights[0]->colour);
    waterProg.setUniform("lights[0].textureID", 0);
    waterProg.setUniform("lights[0].transform", pointLights[0]->transform);
    waterProg.setUniform("lights[0].far_plane", pointLights[0]->far);

    waterProg.setUniform("lights[1].lightIntensity", pointLights[1]->intensity);
    waterProg.setUniform("lights[1].lightColour", pointLights[1]->colour);
    waterProg.setUniform("lights[1].textureID", 1);
    waterProg.setUniform("lights[1].transform", pointLights[1]->transform);
    waterProg.setUniform("lights[1].far_plane", pointLights[1]->far);

    waterProg.setUniform("directionalLights[0].lightIntensity", directionalLights[0]->intensity);
    waterProg.setUniform("directionalLights[0].lightColour", directionalLights[0]->colour);
    waterProg.setUniform("directionalLights[0].textureID", 0);
    waterProg.setUniform("directionalLights[0].project", directionalLights[0]->projection);
    waterProg.setUniform("directionalLights[0].transform", directionalLights[0]->view);
    waterProg.setUniform("directionalLights[0].far_plane", directionalLights[0]->far);

    //defining the textures numbers
    waterProg.setUniform("colourTexture", 0);
    waterProg.setUniform("normalMap", 1);
    waterProg.setUniform("pointDepthMaps", 2);
    waterProg.setUniform("directionalDepthMaps", 3);
    waterProg.setUniform("skybox", 4);

    skyboxProg.use();
    skyboxProg.setUniform("view", sceneCamera.view);
    skyboxProg.setUniform("projection", sceneCamera.projection);
    skyboxProg.setUniform("skybox", 4);

    skyboxProg.setUniform("directionalLights[0].lightIntensity", directionalLights[0]->intensity);
    skyboxProg.setUniform("directionalLights[0].lightColour", directionalLights[0]->colour);
    skyboxProg.setUniform("directionalLights[0].textureID", 0);
    skyboxProg.setUniform("directionalLights[0].project", directionalLights[0]->projection);
    skyboxProg.setUniform("directionalLights[0].transform", directionalLights[0]->view);
    skyboxProg.setUniform("directionalLights[0].far_plane", directionalLights[0]->far);
    skyboxProg.setUniform("directionalDepthMaps", 3);

}

void SceneBasic_Uniform::compile()
{
	try {
        shadowProg.compileShader("omniDepthShader.vert");
        shadowProg.compileShader("omniDepthShader.geom");
        shadowProg.compileShader("omniDepthShader.frag");
        shadowProg.link();

        directionalShadowProg.compileShader("omniDepthShader.vert");
        directionalShadowProg.compileShader("directionalDepthShader.geom");
        directionalShadowProg.compileShader("omniDepthShader.frag");
        directionalShadowProg.link();

		prog.compileShader("shader/basic_uniform.vert");
        prog.compileShader("shader/basic_uniform.geom");
        prog.compileShader("shader/common.frag");
		prog.compileShader("shader/basic_uniform.frag");
		prog.link();

        waterProg.compileShader("shader/waterShader.vert");
        waterProg.compileShader("shader/basic_uniform.geom");
        waterProg.compileShader("shader/common.frag");
        waterProg.compileShader("shader/waterShader.frag");
        waterProg.link();

        skyboxProg.compileShader("shader/skyboxShader.vert");
        skyboxProg.compileShader("shader/skyboxShader.frag");
        //skyboxProg.compileShader("shader/post.frag");
        skyboxProg.link();

	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SceneBasic_Uniform::update( float t )
{
    time = t;

    sceneCamera.view =
        glm::translate(glm::mat4(1.0), vec3(0, -5, -30.0)) *
        glm::rotate(glm::mat4(1.0), glm::radians(30.0f), vec3(1, 0, 0)) *
        glm::rotate(glm::mat4(1.0), glm::radians(t*45.f), vec3(0, 1, 0));
}

void SceneBasic_Uniform::render()
{
    //prepare viewport for shadow rendering
    glViewport(0, 0, SHADOW_RESOLUTION, SHADOW_RESOLUTION);
    //bind the shadow frame buffer rather than displaying in window (?)
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    //clear depth buffer (for shadows to write over)
    glClear(GL_DEPTH_BUFFER_BIT);

    //switch over to shadow shader program
    shadowProg.use();

    //move a light around
    pointLights[0]->transform = glm::translate(glm::mat4(1.0), glm::vec3(glm::sin(time) * -10.0f, -4,  9.0f)); //glm::cos(time) * -10.0f));
    //update moving light uniform
    shadowProg.setUniform("lights[0].transform", pointLights[0]->transform);

    for (Model* model : sceneModels) {
        shadowProg.setUniform("model", model->transform);

        if (model->program == &waterProg) {
            shadowProg.setUniform("time", time);
            shadowProg.setUniform("isWater", true);
        }
        else shadowProg.setUniform("isWater", false);

        //render point light cube depth map
        glBindVertexArray(model->vaoHandle);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->vboHandles[1]);
        glDrawElements(GL_TRIANGLES, model->indicesCount, GL_UNSIGNED_INT, 0);
    }
    //bind cubemaps to TEXTURE2
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, texCubeArray);


    //unbind point lights frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    //set viewport to shadow resolution (for directional this time though)
    glViewport(0, 0, SHADOW_RESOLUTION, SHADOW_RESOLUTION);
    //bind and clear directional buffer
    glBindFramebuffer(GL_FRAMEBUFFER, dirShadowFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    //switch to directional shader
    directionalShadowProg.use();
    //render backfaces depths
    //glCullFace(GL_FRONT);
    for (Model* model : sceneModels) {
        directionalShadowProg.setUniform("model", model->transform);

        if (model->program == &waterProg) {
            directionalShadowProg.setUniform("time", time);
            directionalShadowProg.setUniform("isWater", true);
        }
        else directionalShadowProg.setUniform("isWater", false);

        //render directional light depthmap
        glBindVertexArray(model->vaoHandle);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->vboHandles[1]);
        glDrawElements(GL_TRIANGLES, model->indicesCount, GL_UNSIGNED_INT, 0);
    }
    //bind directional to TEXTURE3
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D_ARRAY, directionalTexArray);


    //unbind fbo and texture
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, width, height);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glClear(GL_DEPTH_BUFFER_BIT);

    //glDepthMask(GL_FALSE);
    //skybox->program->use();
    //skybox->program->setUniform("view", sceneCamera.view);
    //glBindVertexArray(skybox->vaoHandle);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skybox->vboHandles[1]);
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    //glDepthMask(GL_TRUE);
    
    table->transform = glm::translate(glm::mat4(1.0), vec3(cos(time)*10, sin(cos(time) * 10 / 2.f + time * 2.f), -5)) * glm::scale(glm::mat4(1.0), vec3(3));
    ball->transform =
        glm::translate(glm::mat4(1.0), vec3(-2, sin(-2/2.f + time * 2.f), 5));

    for (Model* model : sceneModels) {
        model->program->use();
        model->program->setUniform("view", sceneCamera.view);
        model->program->setUniform("lights[0].transform", pointLights[0]->transform);

        model->program->setUniform("time", time);
        model->drawModel();
        /*//use this model's associated shading
        model->program->use();
        //update mvp uniforms
        model->program->setUniform("view", sceneCamera.view);

        model->program->setUniform("model", model->transform);

        //update point lights info
        model->program->setUniform("lights[0].transform", pointLights[0]->transform);

        //update time
        model->program->setUniform("time", time);

        //set material uniforms from model info
        model->program->setUniform("mtl.ambientReflectivity", model->mtl.ambientReflectivity);
        model->program->setUniform("mtl.diffuseReflectivity", model->mtl.diffuseReflectivity);
        model->program->setUniform("mtl.specularReflectivity", model->mtl.specularReflectivity);
        model->program->setUniform("mtl.specularPower", model->mtl.specularPower);

        //set shading flags
        model->program->setUniform("mtl.shadeFlat", model->mtl.shadeFlat);
        model->program->setUniform("mtl.perFragment", model->mtl.perFragment);

        //bind colour texture to TEXTURE0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, model->colourTexture.handle);

        //bind normal map to TEXTURE1
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, model->normalMap.handle);

        //draw model
        glBindVertexArray(model->vaoHandle);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->vboHandles[1]);
        glDrawElements(GL_TRIANGLES, model->indicesCount, GL_UNSIGNED_INT, 0);*/
    }
    glDepthFunc(GL_LEQUAL);
    skybox->program->use();
    skybox->program->setUniform("view", sceneCamera.view);
    glBindVertexArray(skybox->vaoHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skybox->vboHandles[1]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    //unbind
    glBindVertexArray(0);
}

void SceneBasic_Uniform::resize(int w, int h)
{
    width = w;
    height = h;
    glViewport(0,0,w,h);
}