#version 460

layout (location = 0) in vec3 VertexPosition; // local
layout (location = 2) in vec3 VertexNormal; // local
layout (location = 3) in vec2 VertexTextureCoord;

out vec3 vPos; // world
out vec3 vNor; // local
out vec2 vTex;

// uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPos = model*vec4(VertexPosition,1);
    vPos = worldPos.xyz;
    vNor = VertexNormal;
    vTex = VertexTextureCoord;
    gl_Position = projection*view*worldPos;
}