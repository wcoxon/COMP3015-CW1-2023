#version 460

//takes model vertices and transform, sets gl_position to world/model space of vertex
layout (location = 0) in vec3 aPos;

uniform mat4 model;

uniform bool isWater;
uniform float time;

float wavelength = 2;
float speed = 2;
float amplitude = 1;

void main()
{
    if(isWater){
        
        //do displacement stuff
        gl_Position = model * vec4(aPos,1);

        vec3 waveDisplacement = vec3(0,amplitude*sin(gl_Position.x/wavelength+time*speed),0);

        gl_Position += vec4(waveDisplacement,0);
        //gl_Position = model * vec4(waveDisplacement+aPos, 1.0);


    }
    else{
        //wouldn't this affect culling? not for geometry shader, and then that does the actual gl_positions there
        gl_Position = model * vec4(aPos, 1);


    }
}