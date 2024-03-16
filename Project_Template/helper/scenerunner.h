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



Scene* mainScene;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    //update viewport size when resizing the window
    // 
    //mainCamera->projection = glm::perspective(45.0f, (float)width / (float)height, 1.0f, mainCamera->far);
    //glViewport(0, 0, width, height);
    //mainScene->width = width;
    //mainScene->height = height;
    //mainScene->sceneCamera.projection = glm::perspective(45.0f, (float)width / (float)height, 1.0f, mainScene->sceneCamera.far);
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

        double lastFrameTime = glfwGetTime();

        Model* boat = scene.sceneModels[4];

        float boatAcceleration = 20.f;
        float boatDeceleration = -20.0f;

        vec3 boatVelocity = vec3(0);


        while( ! glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE) ) {
            GLUtils::checkForOpenGLError(__FILE__,__LINE__);
			
            //calculating delta time
            double currentFrameTime = glfwGetTime();
            double deltaTime = currentFrameTime - lastFrameTime;
            lastFrameTime = currentFrameTime;

            //movement stuff
            vec3 inputVector = getInputVector(window);

            vec3 directionVector = (glm::transpose(scene.sceneCamera.view) * glm::vec4(inputVector,0));
            directionVector.y = 0;

            vec3 accelerationVector;

            if (directionVector != vec3(0)) {
                // accelerate

                directionVector = glm::normalize(directionVector);

                accelerationVector = (float)deltaTime * boatAcceleration * directionVector;

                boatVelocity += accelerationVector;

                boat->direction = glm::normalize(boatVelocity);
            }
            else if(boatVelocity!=vec3(0)){
                //slow down

                boatVelocity = glm::normalize(boatVelocity) * std::max(glm::length(boatVelocity)+boatDeceleration*(float)deltaTime,0.f);

            }

            vec3 movementVector = (float)deltaTime * boatVelocity;

            boat->translate(movementVector);


            if (glfwGetKey(window, GLFW_KEY_1)) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            
            if (glfwGetKey(window, GLFW_KEY_2)) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            

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
