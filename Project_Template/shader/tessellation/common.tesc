#version 460

layout (vertices=4) out;

in vec3 vPos[]; // local
in vec3 vNor[]; // local
in vec2 vTex[];

out vec3 tcPos[]; // local
out vec3 tcNor[]; // local
out vec2 tcTex[];

// uniforms
uniform mat4 view; // SCENE CAMERA view

uniform vec3 boatPosition;

// parameters
const int MIN_TESS_LEVEL = 1;
const int MAX_TESS_LEVEL = 8;

const float MIN_DISTANCE = 0; //interpolate tessellation from this distance
const float MAX_DISTANCE = 50; //interpolate tessellation to this distance


void calculateTessLevels(){

    vec4 viewPos00 =  view * gl_in[0].gl_Position;
    vec4 viewPos01 =  view * gl_in[1].gl_Position;
    vec4 viewPos10 =  view * gl_in[2].gl_Position;
    vec4 viewPos11 =  view * gl_in[3].gl_Position;

    float viewDist00 = abs(viewPos00.z);
    float viewDist01 = abs(viewPos01.z);
    float viewDist10 = abs(viewPos10.z);
    float viewDist11 = abs(viewPos11.z);

    float boatDist00 = distance(boatPosition, gl_in[0].gl_Position.xyz);
    float boatDist01 = distance(boatPosition, gl_in[1].gl_Position.xyz);
    float boatDist10 = distance(boatPosition, gl_in[2].gl_Position.xyz);
    float boatDist11 = distance(boatPosition, gl_in[3].gl_Position.xyz);

    float distance00 = min(viewDist00,boatDist00);
    float distance01 = min(viewDist01,boatDist01);
    float distance10 = min(viewDist10,boatDist10);
    float distance11 = min(viewDist11,boatDist11);

    float factor00 = clamp((distance00-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
    float factor01 = clamp((distance01-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
    float factor10 = clamp((distance10-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
    float factor11 = clamp((distance11-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);

    float tessLevels[4] = {
        mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(factor10, factor00) ),
        mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(factor00, factor01) ),
        mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(factor01, factor11) ),
        mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(factor11, factor10) )
    };


    gl_TessLevelOuter = tessLevels;

    gl_TessLevelInner[0] = max(gl_TessLevelOuter[1], gl_TessLevelOuter[3]);
    gl_TessLevelInner[1] = max(gl_TessLevelOuter[0], gl_TessLevelOuter[2]);
}
