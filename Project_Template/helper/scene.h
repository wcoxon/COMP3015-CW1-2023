#pragma once


#include <glm/glm.hpp>

#include <iostream>
#include <string>
#include <vector>

#include "Camera.h"
#include "Mesh.h";

#include "DirectionalLight.h"
#include "PointLight.h"

using std::vector;

class Scene
{
protected:
	glm::mat4 model, view, projection;

public:
    int width;
    int height;


    Model skybox;

    Camera sceneCamera;
    vector<Model*> sceneModels;

    vector<PointLight*> pointLights;
    vector<DirectionalLight*> directionalLights;

	Scene() : m_animate(true), width(800), height(600) { }
	virtual ~Scene() {}

	void setDimensions( int w, int h ) {
	    width = w;
	    height = h;
	}
	
    /**
      Load textures, initialize shaders, etc.
      */
    virtual void initScene() = 0;

    /**
      This is called prior to every frame.  Use this
      to update your animation.
      */
    virtual void update( float t ) = 0;

    /**
      Draw your scene.
      */
    virtual void render() = 0;

    /**
      Called when screen is resized
      */
    virtual void resize(int, int) = 0;
    
    void animate( bool value ) { m_animate = value; }
    bool animating() { return m_animate; }
    
protected:
	bool m_animate;
};
