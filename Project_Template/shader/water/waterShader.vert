#version 460

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNor;
layout (location = 3) in vec2 aTex;

out vec3 vPos;
out vec3 vNor;
out vec2 vTex;
out vec3 vColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vPos = aPos;
    vNor = aNor;
    vTex = aTex;
    vColor = vec3(1);
    
    gl_Position = vec4(vPos,1);
}