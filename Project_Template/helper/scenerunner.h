#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glutils.h"

#include "glm/glm.hpp"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#include <map>
#include <string>
#include <fstream>
#include <iostream>

#include "scene.h"
#include "Camera.h"

using glm::vec3;


#define GAMMA_CORRECTION_ON_KEY GLFW_KEY_G
#define GAMMA_CORRECTION_OFF_KEY GLFW_KEY_H

Scene* mainScene;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    mainScene->resize(width, height);
}



class SceneRunner {
private:
    GLFWwindow * window;
    int fbw, fbh;
	bool debug;           // Set true to enable debug messages

public:
    SceneRunner(const std::string & windowTitle, int width = WIN_WIDTH, int height = WIN_HEIGHT, int samples = 0) : debug(true) {
        // Initialize GLFW
        if( !glfwInit() ) exit( EXIT_FAILURE );

#ifdef __APPLE__
        // Select OpenGL 4.1
        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );
#else
        // Select OpenGL 4.6
        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
#endif
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        //glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); //whyy
        
        if(debug) 
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
        if(samples > 0) {
            glfwWindowHint(GLFW_SAMPLES, samples);
        }

        // Open the window
        window = glfwCreateWindow( WIN_WIDTH, WIN_HEIGHT, windowTitle.c_str(), NULL, NULL );
        if( ! window ) {
			std::cerr << "Unable to create OpenGL context." << std::endl;
            glfwTerminate();
            exit( EXIT_FAILURE );
        }
        glfwMakeContextCurrent(window);

        // Get framebuffer size
        glfwGetFramebufferSize(window, &fbw, &fbh);

        

        // Load the OpenGL functions.
        if(!gladLoadGL()) { exit(-1); }

        GLUtils::dumpGLInfo();

        // Initialization
        glClearColor(0.5f,0.5f,0.5f,1.0f);
#ifndef __APPLE__
		if (debug) {
			glDebugMessageCallback(GLUtils::debugCallback, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
			glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0,
				GL_DEBUG_SEVERITY_NOTIFICATION, -1, "Start debugging");
		}
#endif
    }

    int run(Scene & scene) {
        
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        scene.setDimensions(fbw, fbh);
        scene.initScene();
        scene.resize(fbw, fbh);

        // Enter the main loop
        mainLoop(window, scene);

#ifndef __APPLE__
		if( debug )
			glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 1,
				GL_DEBUG_SEVERITY_NOTIFICATION, -1, "End debug");
#endif

		// Close window and terminate GLFW
		glfwTerminate();

        // Exit program
        return EXIT_SUCCESS;
    }

    static std::string parseCLArgs(int argc, char ** argv, std::map<std::string, std::string> & sceneData) {
        if( argc < 2 ) {
            printHelpInfo(argv[0], sceneData);
            exit(EXIT_FAILURE);
        }

        std::string recipeName = argv[1];
        auto it = sceneData.find(recipeName);
        if( it == sceneData.end() ) {
            printf("Unknown recipe: %s\n\n", recipeName.c_str());
            printHelpInfo(argv[0], sceneData);
            exit(EXIT_FAILURE);
        }

        return recipeName;
    }

private:
    static void printHelpInfo(const char * exeFile,  std::map<std::string, std::string> & sceneData) {
        printf("Usage: %s recipe-name\n\n", exeFile);
        printf("Recipe names: \n");
        for( auto it : sceneData ) {
            printf("  %11s : %s\n", it.first.c_str(), it.second.c_str());
        }
    }


    vec3 getInputVector(GLFWwindow* window) {

        vec3 inputVector = vec3(0);

        if (glfwGetKey(window, GLFW_KEY_W)) {
            inputVector.z -= 1;
        }
        if (glfwGetKey(window, GLFW_KEY_S)) {
            inputVector.z += 1;
        }
        if (glfwGetKey(window, GLFW_KEY_A)) {
            inputVector.x -= 1;
        }
        if (glfwGetKey(window, GLFW_KEY_D)) {
            inputVector.x += 1;
        }

        if (inputVector != vec3(0)) inputVector = glm::normalize(inputVector);

        return inputVector;
    }

    void mainLoop(GLFWwindow * window, Scene & scene) {

        mainScene = &scene;

        Model* boat = scene.sceneModels[4];
        Model* water = scene.sceneModels[0];

        float angularAcceleration = 1.0f;

        float boatAcceleration = 50.f;
        float boatDeceleration = -50.0f;

        float maxSpeed = 50.f;

        vec3 boatVelocity = vec3(0);

        float boatTurnSpeed = 0;

        double lastFrameTime = glfwGetTime();

        vec3 cameraArm = vec3(0, 5, 10);

        while( ! glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE) ) {
            GLUtils::checkForOpenGLError(__FILE__,__LINE__);
			
            //calculating delta time
            double currentFrameTime = glfwGetTime();
            double deltaTime = currentFrameTime - lastFrameTime;
            lastFrameTime = currentFrameTime;

            //movement stuff
            vec3 inputVector = getInputVector(window);

            vec3 directionVector = (glm::transpose(scene.sceneCamera.view) * glm::vec4(inputVector,0)); // turn input into world direction
            directionVector.y = 0; //only direct boat on x z plane

            if (directionVector != vec3(0) && glm::length(boatVelocity)<maxSpeed) {
                //rotate
                vec3 localRight = vec3(boat->transform*glm::vec4(1, 0, 0, 0));
                float turnScale = inputVector.x;//glm::dot(directionVector, localRight);

                boat->direction = vec3(glm::rotate(glm::mat4(1), -turnScale * (float)deltaTime, vec3(0, 1, 0))*glm::vec4(boat->direction,0));

                // accelerate
                float accelerationScale = -inputVector.z;//glm::dot(directionVector, boat->direction);


                directionVector = glm::normalize(directionVector);

                vec3 accelerationVector = boat->direction * accelerationScale * (float)deltaTime * boatAcceleration;//(float)deltaTime * boatAcceleration * directionVector;

                boatVelocity += accelerationVector;

                //boat->direction = glm::normalize(boatVelocity);
            }
            if(boatVelocity!=vec3(0)){
                float deceleration = 0.1f*std::pow(glm::length(boatVelocity),2);
                //slow down
                boatVelocity = glm::normalize(boatVelocity) * std::max(glm::length(boatVelocity)-deceleration*(float)deltaTime,0.f);

            }
            
            water->program->use();
            water->program->setUniform("boatSpeed", glm::length(boatVelocity));

            vec3 movementVector = (float)deltaTime * boatVelocity;

            boat->translate(movementVector);

            if (glfwGetKey(window, GLFW_KEY_Z)) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            if (glfwGetKey(window, GLFW_KEY_X)) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            if (glfwGetKey(window, GLFW_KEY_3)) {
                boat->program->use();
                boat->program->setUniform("volumetricLighting", true);
                scene.sceneModels[0]->program->use();
                scene.sceneModels[0]->program->setUniform("volumetricLighting", true);
            }
            if (glfwGetKey(window, GLFW_KEY_4)) {
                boat->program->use();
                boat->program->setUniform("volumetricLighting", false);
                scene.sceneModels[0]->program->use();
                scene.sceneModels[0]->program->setUniform("volumetricLighting", false);
            }

            if (glfwGetKey(window, GLFW_KEY_UP)) {
                cameraArm.z -= 15 * deltaTime;
            }
            if (glfwGetKey(window, GLFW_KEY_DOWN)) {
                cameraArm.z += 15*deltaTime;
            }
            if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
                cameraArm.x += 15 * deltaTime;
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT)) {
                cameraArm.x -= 15 * deltaTime;
            }
            if (glfwGetKey(window, GLFW_KEY_SPACE)) {
                cameraArm.y += 15 * deltaTime;
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) {
                cameraArm.y -= 15 * deltaTime;
            }

            if (glfwGetKey(window, GAMMA_CORRECTION_ON_KEY)) {
                boat->program->use();
                boat->program->setUniform("gammaCorrection", true);
                water->program->use();
                water->program->setUniform("gammaCorrection", true);
                scene.skybox.program->use();
                scene.skybox.program->setUniform("gammaCorrection", true);
            }
            if (glfwGetKey(window, GAMMA_CORRECTION_OFF_KEY)) {
                boat->program->use();
                boat->program->setUniform("gammaCorrection", false);
                water->program->use();
                water->program->setUniform("gammaCorrection", false);
                scene.skybox.program->use();
                scene.skybox.program->setUniform("gammaCorrection", false);
            }

            boat->updateMatrix();
            vec3 targetPosition = boat->position + vec3(boat->transform * glm::vec4(cameraArm, 0));
            scene.sceneCamera.position = glm::mix(scene.sceneCamera.position, targetPosition, deltaTime * 2);
            scene.sceneCamera.view = glm::lookAt(scene.sceneCamera.position, boat->position, vec3(0, 1, 0));

            scene.update(float(glfwGetTime()));
            scene.render();
            glfwSwapBuffers(window);

            glfwPollEvents();
			int state = glfwGetKey(window, GLFW_KEY_SPACE);
			if (state == GLFW_PRESS)
				scene.animate(!scene.animating());
        }
    }
};
