#version 460


layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexColor;
layout (location = 2) in vec3 VertexNormal;
layout (location = 3) in vec2 VertexTextureCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform sampler2D displacementMap;

uniform bool perFragment;

out vec3 vColor;

out vec3 vPos;
out vec3 vNor;
out vec2 vTex;

void main(){

    vColor = vec3(1);

    //changed from vertexPosition, I'm trying to send worldspace position out
    vPos = (model*vec4(VertexPosition,1)).xyz;
    vNor = VertexNormal;
    vTex = VertexTextureCoord;

    gl_Position = projection*view*vec4(vPos,1);//model*vec4(vPos,1);
    

}