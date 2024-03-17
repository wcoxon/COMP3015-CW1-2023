#version 460

// specify number of control points per patch output
// this value controls the size of the input and output arrays
layout (vertices=4) out;

// varying input from vertex shader
in vec3 vPos[];
in vec3 vNor[];
in vec2 vTex[];
in vec3 vColor[];

// varying output to evaluation shader
out vec3 tcPos[];
out vec3 tcNor[];
out vec2 tcTex[];
out vec3 tcColor[];


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 boatPosition;

void main()
{
    // ----------------------------------------------------------------------
    // pass attributes through
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    tcPos[gl_InvocationID] = vPos[gl_InvocationID];
    tcNor[gl_InvocationID] = vNor[gl_InvocationID];
    tcTex[gl_InvocationID] = vTex[gl_InvocationID];
    tcColor[gl_InvocationID] = vColor[gl_InvocationID];

    // ----------------------------------------------------------------------
    // invocation zero controls tessellation levels for the entire patch
    if (gl_InvocationID == 0)
    {
        const int MIN_TESS_LEVEL = 1;
        const int MAX_TESS_LEVEL = 8;
        const float MIN_DISTANCE = 0;
        const float MAX_DISTANCE = 30;

        vec4 worldPos00 =  model * gl_in[0].gl_Position;
        vec4 worldPos01 =  model * gl_in[1].gl_Position;
        vec4 worldPos10 =  model * gl_in[2].gl_Position;
        vec4 worldPos11 =  model * gl_in[3].gl_Position;
        // ----------------------------------------------------------------------
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

        // ----------------------------------------------------------------------
        // Step 3: "distance" from camera scaled between 0 and 1
        float factor00 = clamp((distance00-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
        float factor01 = clamp((distance01-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
        float factor10 = clamp((distance10-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
        float factor11 = clamp((distance11-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);

        // ----------------------------------------------------------------------
        // Step 4: interpolate edge tessellation level based on closer vertex
        float tessLevel0 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(factor10, factor00) );
        float tessLevel1 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(factor00, factor01) );
        float tessLevel2 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(factor01, factor11) );
        float tessLevel3 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(factor11, factor10) );

        // ----------------------------------------------------------------------
        // Step 5: set the corresponding outer edge tessellation levels
        gl_TessLevelOuter[0] = tessLevel0;
        gl_TessLevelOuter[1] = tessLevel1;
        gl_TessLevelOuter[2] = tessLevel2;
        gl_TessLevelOuter[3] = tessLevel3;

        // ----------------------------------------------------------------------
        // Step 6: set the inner tessellation levels to the max of the two parallel edges
        gl_TessLevelInner[0] = max(tessLevel1, tessLevel3);
        gl_TessLevelInner[1] = max(tessLevel0, tessLevel2);
    
    }
}