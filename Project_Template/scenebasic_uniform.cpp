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

using std::vector;
using glm::vec3;
using glm::mat4;

SceneBasic_Uniform::SceneBasic_Uniform() : time(0.0f) {}


unsigned int loadCubemap(vector<std::string> faces)
{
    stbi_set_flip_vertically_on_load(false);
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


void generatePatches(Model* model) {
    vector<vec3> quadVerts = {};
    vector<vec3> quadNormals = {};
    vector<glm::vec2> quadTexCoords = {};
    vector<GLuint> quadIndices = {};

    float patchesX = 32;
    float patchesZ = 32;

    for (int patchZ = 0; patchZ < patchesZ; patchZ++) {
        for (int patchX = 0; patchX < patchesX; patchX++) {
            float patchLeft = -patchesX / 2.0f + patchX;
            float patchRight = patchLeft + 1;

            float patchTop = -patchesZ / 2.0f + patchZ;
            float patchBottom = patchTop + 1;

            vec3 patchBL = { patchLeft,0,patchBottom };
            vec3 patchBR = { patchRight,0,patchBottom };
            vec3 patchTL = { patchLeft,0,patchTop };
            vec3 patchTR = { patchRight,0,patchTop };

            quadIndices.push_back(quadVerts.size());
            quadVerts.push_back(patchBL);
            quadNormals.push_back({ 0,1,0 });
            quadTexCoords.push_back({ 0,0 });


            quadIndices.push_back(quadVerts.size());
            quadVerts.push_back(patchBR);
            quadNormals.push_back({ 0,1,0 });
            quadTexCoords.push_back({ 1,0 });


            quadIndices.push_back(quadVerts.size());
            quadVerts.push_back(patchTL);
            quadNormals.push_back({ 0,1,0 });
            quadTexCoords.push_back({ 0,1 });


            quadIndices.push_back(quadVerts.size());
            quadVerts.push_back(patchTR);
            quadNormals.push_back({ 0,1,0 });
            quadTexCoords.push_back({ 1,1 });
        }
    }

    model->indicesCount = quadIndices.size();
    model->loadBufferData(quadVerts, quadNormals, quadTexCoords, quadIndices);
    model->drawMode = GL_PATCHES;
}

//Model* skybox;

Model* water;

Model* ball;
Model* table;
Model* wall;
Model* boat;

//flat shading, ambient, diffuse, specular, power, perfragment
Material waterMaterial{ false, .5f, .5f, .9f, 64 };
Material wallMaterial{ true, .5f, 1.f, 0.0f };

Texture* foamTexture;

float amplitude = 2;
float wavelength = 8;
float waveSpeed = 2;
glm::vec3 waveDirection = vec3(1, 0, 0);

void SceneBasic_Uniform::initScene()
{
    glFrontFace(GL_CW);
    glEnable(GL_DEPTH_TEST);
    glPatchParameteri(GL_PATCH_VERTICES, 4);

    compile();

    water = new Model();
    water->program = &waterProg;
    generatePatches(water);
    water->colourTexture.load("./media/textures/seaTexture - 56_sea water foam texture-seamless.jpg");
    water->normalMap.load("./media/textures/0001.png");
    water->scale = vec3(15);
    water->mtl = waterMaterial;
    sceneModels.push_back(water);

    foamTexture = new Texture();
    foamTexture->load("./media/textures/super perlin 2 - 512x512.png");

    wall = new Model();
    wall->program = &prog;
    wall->loadFileModel("./media/wall.obj");
    wall->colourTexture.load("./media/textures/pebblesC.jpg");
    wall->normalMap.load("./media/textures/pebblesNormal.jpg");
    wall->transform = glm::translate(glm::mat4(1.0), vec3(-1, -1, -20)) *glm::rotate(glm::mat4(1.0), glm::radians(-90.f), vec3(0, 1, 0))* glm::scale(glm::mat4(1.0), vec3(5));
    wall->mtl = wallMaterial;
    sceneModels.push_back(wall);
    
    table = new Model();
    table->program = &prog;
    table->loadFileModel("./media/table.obj");
    table->colourTexture.load("./media/textures/wood.jpg");
    table->transform = glm::translate(glm::mat4(1.0), vec3(-1, 0, -5)) * glm::scale(glm::mat4(1.0), vec3(3));
    table->mtl = wallMaterial;
    sceneModels.push_back(table);

    ball = new Model();
    ball->program = &prog;
    ball->loadFileModel("./media/ball.obj");
    ball->position = vec3(-2, 1, 5);
    ball->scale = vec3(5.0);
    ball->mtl.shadeFlat = false;
    ball->mtl.perFragment = false;
    sceneModels.push_back(ball);

    boat = new Model();
    boat->program = &prog;
    boat->loadFileModel("./media/boat3.obj");
    boat->colourTexture.load("./media/textures/boatTexture.png");
    boat->position = vec3(2, 1, 3);
    boat->scale = vec3(1.5);
    boat->mtl.shadeFlat = true;
    sceneModels.push_back(boat);

    sceneModels.push_back(new Model());
    sceneModels.back()->program = &prog;
    sceneModels.back()->loadFileModel("./media/wall.obj");
    sceneModels.back()->colourTexture.load("./media/textures/pebblesC.jpg");
    sceneModels.back()->normalMap.load("./media/textures/pebblesNormal.jpg");
    sceneModels.back()->transform = glm::translate(glm::mat4(1.0), vec3(20, -1, -10)) * glm::scale(glm::mat4(1.0), vec3(5));
    sceneModels.back()->mtl = wallMaterial;

    skybox.program = &skyboxProg;
    vector<vec3> skyquadVerts = { {-1,-1,-1},{-1,1,-1} ,{1,-1,-1} ,{1,1,-1} };
    vector<GLuint> skyquadIndices = { 0,1,2, 1,2,3 };
    vector<std::string> skyboxTexturePaths = { "./media/textures/skybox/right.jpg","./media/textures/skybox/left.jpg","./media/textures/skybox/top.jpg","./media/textures/skybox/bottom.jpg","./media/textures/skybox/front.jpg","./media/textures/skybox/back.jpg" };

    glBindVertexArray(skybox.vaoHandle);
    glBindBuffer(GL_ARRAY_BUFFER, skybox.vboHandles[0]);
    glBufferData(GL_ARRAY_BUFFER, skyquadVerts.size() * sizeof(vec3), skyquadVerts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skybox.vboHandles[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, skyquadIndices.size() * sizeof(GLuint), skyquadIndices.data(), GL_STATIC_DRAW);
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
    glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 1, GL_DEPTH_COMPONENT24, POINT_SHADOW_RESOLUTION, POINT_SHADOW_RESOLUTION, numLights * 6);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texCubeArray, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0); //unbind fbo
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, 0); //unbind texture

    glm::mat4 shadowProj = glm::perspective(glm::half_pi<float>(), 1.f, 0.f, 20.0f);

    pointLights.push_back(new PointLight());
    pointLights[0]->transform = glm::translate(glm::mat4(1.0), vec3(-3, -4, -3));
    pointLights[0]->intensity = 25;
    pointLights.push_back(new PointLight());
    pointLights[1]->transform = glm::translate(glm::mat4(1.0),vec3(8,-4,-5));
    pointLights[1]->colour = vec3(0,0.5,1);
    pointLights[1]->intensity = 25;


    glGenFramebuffers(1, &dirShadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, dirShadowFBO);
    glGenTextures(1, &directionalTexArray);
    glBindTexture(GL_TEXTURE_2D_ARRAY, directionalTexArray);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH_COMPONENT24, DIRECTIONAL_SHADOW_RESOLUTION, DIRECTIONAL_SHADOW_RESOLUTION, 1);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, directionalTexArray, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("Error: framebuffer is not complete!");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0); //unbind fbo
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0); //unbind texture

    directionalLights.push_back(new DirectionalLight());
    directionalLights[0]->view = glm::lookAt(vec3(-20.0f, 20.0f, 0.f), vec3(0.0f, 0.0f, 0.0f), vec3(0.f, 1.f, 0.f));

    shadowProg.use();
    shadowProg.printActiveUniforms();
    shadowProg.setUniform("lights[0].project", shadowProj);
    shadowProg.setUniform("lights[0].transform", pointLights[0]->transform);
    shadowProg.setUniform("lights[1].project", shadowProj);
    shadowProg.setUniform("lights[1].transform", pointLights[1]->transform);
    shadowProg.setUniform("far_plane", pointLights[0]->far);

    directionalShadowProg.use();
    directionalShadowProg.printActiveUniforms();
    directionalShadowProg.setUniform("lights[0].transform", directionalLights[0]->view);
    directionalShadowProg.setUniform("lights[0].project", directionalLights[0]->projection);
    directionalShadowProg.setUniform("far_plane", directionalLights[0]->far);

    prog.use();
    cout << "standard shader:" << endl;
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

    waterProg.use();
    cout << "water shader:" << endl;
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
    waterProg.setUniform("wavelength", wavelength);
    waterProg.setUniform("waveSpeed", waveSpeed);
    waterProg.setUniform("amplitude", amplitude);

    waterDirectionalShadowPass.use();
    waterDirectionalShadowPass.setUniform("lights[0].transform", directionalLights[0]->view);
    waterDirectionalShadowPass.setUniform("lights[0].project", directionalLights[0]->projection);
    waterDirectionalShadowPass.setUniform("far_plane", directionalLights[0]->far);
    waterDirectionalShadowPass.setUniform("wavelength", wavelength);
    waterDirectionalShadowPass.setUniform("waveSpeed", waveSpeed);
    waterDirectionalShadowPass.setUniform("amplitude", amplitude);

    waterPointShadowPass.use();
    waterPointShadowPass.setUniform("lights[0].project", shadowProj);
    waterPointShadowPass.setUniform("lights[0].transform", pointLights[0]->transform);
    waterPointShadowPass.setUniform("lights[1].project", shadowProj);
    waterPointShadowPass.setUniform("lights[1].transform", pointLights[1]->transform);
    waterPointShadowPass.setUniform("far_plane", 20.0f);
    waterPointShadowPass.setUniform("wavelength", wavelength);
    waterPointShadowPass.setUniform("waveSpeed", waveSpeed);
    waterPointShadowPass.setUniform("amplitude", amplitude);

    skyboxProg.use();
    skyboxProg.setUniform("view", sceneCamera.view);
    skyboxProg.setUniform("projection", sceneCamera.projection);
    skyboxProg.setUniform("directionalLights[0].lightIntensity", directionalLights[0]->intensity);
    skyboxProg.setUniform("directionalLights[0].lightColour", directionalLights[0]->colour);
    skyboxProg.setUniform("directionalLights[0].textureID", 0);
    skyboxProg.setUniform("directionalLights[0].project", directionalLights[0]->projection);
    skyboxProg.setUniform("directionalLights[0].transform", directionalLights[0]->view);
    skyboxProg.setUniform("directionalLights[0].far_plane", directionalLights[0]->far);
}

void SceneBasic_Uniform::compile()
{
	try {
        shadowProg.compileShader("shader/lightmapping/omniDepthShader.vert");
        shadowProg.compileShader("shader/lightmapping/omniDepthShader.geom");
        shadowProg.compileShader("shader/lightmapping/omniDepthShader.frag");
        shadowProg.link();

        directionalShadowProg.compileShader("shader/lightmapping/omniDepthShader.vert");
        directionalShadowProg.compileShader("shader/lightmapping/directionalDepthShader.geom");
        directionalShadowProg.compileShader("shader/lightmapping/omniDepthShader.frag");
        directionalShadowProg.link();

		prog.compileShader("shader/standard/basic_uniform.vert");
        prog.compileShader("shader/standard/basic_uniform.geom");
        prog.compileShader("shader/common.frag");
		prog.compileShader("shader/standard/basic_uniform.frag");
		prog.link();

        waterProg.compileShader("shader/water/waterShader.vert");
        waterProg.compileShader("shader/tessellation/common.tesc");
        waterProg.compileShader("shader/tessellation/control.tesc");
        waterProg.compileShader("shader/tessellation/common.tese");
        waterProg.compileShader("shader/tessellation/eval.tese");
        waterProg.compileShader("shader/standard/basic_uniform.geom");
        waterProg.compileShader("shader/common.frag");
        waterProg.compileShader("shader/water/waterShader.frag");
        waterProg.link();

        waterDirectionalShadowPass.compileShader("shader/lightmapping/waterDepthShader.vert");
        waterDirectionalShadowPass.compileShader("shader/tessellation/common.tesc");
        waterDirectionalShadowPass.compileShader("shader/tessellation/controlDepth.tesc");
        waterDirectionalShadowPass.compileShader("shader/tessellation/common.tese");
        waterDirectionalShadowPass.compileShader("shader/tessellation/evalDepth.tese");
        waterDirectionalShadowPass.compileShader("shader/lightmapping/directionalDepthShader.geom");
        waterDirectionalShadowPass.compileShader("shader/lightmapping/waterDepthShader.frag");
        waterDirectionalShadowPass.link();

        waterPointShadowPass.compileShader("shader/lightmapping/waterDepthShader.vert");
        waterPointShadowPass.compileShader("shader/tessellation/common.tesc");
        waterPointShadowPass.compileShader("shader/tessellation/controlDepth.tesc");
        waterPointShadowPass.compileShader("shader/tessellation/common.tese");
        waterPointShadowPass.compileShader("shader/tessellation/evalDepth.tese");
        waterPointShadowPass.compileShader("shader/lightmapping/omniDepthShader.geom");
        waterPointShadowPass.compileShader("shader/lightmapping/waterDepthShader.frag");
        waterPointShadowPass.link();


        skyboxProg.compileShader("shader/skybox/skyboxShader.vert");
        skyboxProg.compileShader("shader/skybox/skyboxShader.frag");
        skyboxProg.link();

	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

float getWaveHeight(vec3 position,float time) {
    return amplitude*sin(position.x / wavelength + time * waveSpeed);
}

void SceneBasic_Uniform::update( float t )
{
    time = t;

    pointLights[0]->transform = glm::translate(glm::mat4(1.0), vec3(glm::sin(time) * -10.0f, -9, glm::cos(time) * -10.0f)); //move light around
    
    ball->position.y = getWaveHeight(ball->position, time); //ball bob
    ball->updateMatrix();

    boat->position.y = getWaveHeight(boat->position, time); //boat bob
    boat->setUp(vec3((amplitude / wavelength) * -cos(boat->position.x / wavelength + time * waveSpeed), 1, 0));
    boat->direction = vec3(boat->direction.x, glm::dot(boat->direction,waveDirection) * (amplitude / wavelength) * cos(boat->position.x / wavelength + time * waveSpeed), boat->direction.z);
    boat->updateMatrix();

    water->program->use();
    water->program->setUniform("boatPosition", boat->position);
    water->position = vec3(sceneCamera.position.x, 0, sceneCamera.position.z);
    water->updateMatrix();
}

void SceneBasic_Uniform::render()
{
    glViewport(0, 0, POINT_SHADOW_RESOLUTION, POINT_SHADOW_RESOLUTION); //prepare viewport for shadow rendering (point)
     //bind and clear the point shadows fbo
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    //render scene
    for (Model* model : sceneModels) {
        
        if (model->program == &waterProg) {
            waterPointShadowPass.use();
            waterPointShadowPass.setUniform("view", sceneCamera.view);
            waterPointShadowPass.setUniform("model", model->transform);
            waterPointShadowPass.setUniform("time", time);
            waterPointShadowPass.setUniform("lights[0].transform", pointLights[0]->transform);
            waterPointShadowPass.setUniform("boatPosition", boat->position);
        }
        else {
            shadowProg.use();
            shadowProg.setUniform("model", model->transform);
            shadowProg.setUniform("lights[0].transform", pointLights[0]->transform);
        }

        //render point light cube depth map
        glBindVertexArray(model->vaoHandle);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->vboHandles[1]);
        glDrawElements(model->drawMode, model->indicesCount, GL_UNSIGNED_INT, 0);
    }
    //bind pointlight cubemap to TEXTURE2
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, texCubeArray);
    //unbind fbo
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, DIRECTIONAL_SHADOW_RESOLUTION, DIRECTIONAL_SHADOW_RESOLUTION); //set viewport to shadow resolution (directional)
    //bind and clear directional buffer
    glBindFramebuffer(GL_FRAMEBUFFER, dirShadowFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    //render scene
    for (Model* model : sceneModels) {
        
        if (model->program == &waterProg) {
            waterDirectionalShadowPass.use();
            waterDirectionalShadowPass.setUniform("view", sceneCamera.view);
            waterDirectionalShadowPass.setUniform("model", model->transform);
            waterDirectionalShadowPass.setUniform("time", time);
            waterDirectionalShadowPass.setUniform("boatPosition", boat->position);
            waterDirectionalShadowPass.setUniform("lights[0].transform", directionalLights[0]->view);
        }
        else {
            directionalShadowProg.use();
            directionalShadowProg.setUniform("model", model->transform);
            directionalShadowProg.setUniform("lights[0].transform", directionalLights[0]->view);
        }

        //render directional light depthmap
        glBindVertexArray(model->vaoHandle);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->vboHandles[1]);
        glDrawElements(model->drawMode, model->indicesCount, GL_UNSIGNED_INT, 0);
    }
    //bind directional map to TEXTURE3
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D_ARRAY, directionalTexArray);
    //unbind fbo
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, width, height);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    //render scene
    for (Model* model : sceneModels) {
        model->program->use();
        model->program->setUniform("view", sceneCamera.view);
        model->program->setUniform("projection", sceneCamera.projection);
        model->program->setUniform("lights[0].transform", pointLights[0]->transform);
        model->program->setUniform("directionalLights[0].transform", directionalLights[0]->view);
        model->program->setUniform("time", time);

        if (model->program == &waterProg) {
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_2D, foamTexture->handle);
        }

        model->drawModel();
    }

    //draw skybox
    glDepthFunc(GL_LEQUAL);
    skybox.program->use();
    skybox.program->setUniform("view", sceneCamera.view);
    skybox.program->setUniform("projection", sceneCamera.projection);
    glBindVertexArray(skybox.vaoHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skybox.vboHandles[1]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0); //unbind vao
}

void SceneBasic_Uniform::resize(int w, int h)
{
    width = w;
    height = h;
    sceneCamera.projection = glm::perspective(45.0f, w / (float)h, 1.0f, sceneCamera.far);
}
