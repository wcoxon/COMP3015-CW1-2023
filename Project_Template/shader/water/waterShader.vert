#version 460

layout (location = 0) in vec3 aPos; // local
layout (location = 2) in vec3 aNor; // local
layout (location = 3) in vec2 aTex;

out vec3 vPos; // local
out vec3 vNor; // local
out vec2 vTex;

uniform mat4 model;

void main()
{
    vPos = aPos;
    vNor = aNor;
    vTex = aTex;
    gl_Position = model*vec4(vPos,1);
}