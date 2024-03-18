#version 460

layout (vertices=4) out;

in vec3 vPos[]; // local
in vec3 vNor[]; // local
in vec2 vTex[];

out vec3 tcPos[]; // local
out vec3 tcNor[]; // local
out vec2 tcTex[];

// uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 boatPosition;

const int MIN_TESS_LEVEL = 1;
const int MAX_TESS_LEVEL = 8;

const float MIN_DISTANCE = 0; //interpolate tessellation from this distance
const float MAX_DISTANCE = 50; //interpolate tessellation to this distance

float[4] calculateTessLevels(){

    vec4 worldPos00 = gl_in[0].gl_Position;
    vec4 worldPos01 = gl_in[1].gl_Position;
    vec4 worldPos10 = gl_in[2].gl_Position;
    vec4 worldPos11 = gl_in[3].gl_Position;

    // Step 2: transform each vertex into eye space
    vec4 viewPos00 =  view * worldPos00;
    vec4 viewPos01 =  view * worldPos01;
    vec4 viewPos10 =  view * worldPos10;
    vec4 viewPos11 =  view * worldPos11;


    float viewDist00 = abs(viewPos00.z);
    float viewDist01 = abs(viewPos01.z);
    float viewDist10 = abs(viewPos10.z);
    float viewDist11 = abs(viewPos11.z);

    float boatDist00 = distance(boatPosition,worldPos00.xyz);
    float boatDist01 = distance(boatPosition,worldPos01.xyz);
    float boatDist10 = distance(boatPosition,worldPos10.xyz);
    float boatDist11 = distance(boatPosition,worldPos11.xyz);


    float distance00 = min(viewDist00,boatDist00);
    float distance01 = min(viewDist01,boatDist01);
    float distance10 = min(viewDist10,boatDist10);
    float distance11 = min(viewDist11,boatDist11);

    // Step 3: "distance" from camera scaled between 0 and 1
    float factor00 = clamp((distance00-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
    float factor01 = clamp((distance01-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
    float factor10 = clamp((distance10-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
    float factor11 = clamp((distance11-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);


    float tessLevels[4];

    // Step 4: interpolate edge tessellation level based on closer vertex
    tessLevels[0] = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(factor10, factor00) );
    tessLevels[1] = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(factor00, factor01) );
    tessLevels[2] = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(factor01, factor11) );
    tessLevels[3] = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(factor11, factor10) );

    return tessLevels;
}
