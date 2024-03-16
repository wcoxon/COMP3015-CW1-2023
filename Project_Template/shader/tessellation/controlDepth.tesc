#version 460

// specify number of control points per patch output
// this value controls the size of the input and output arrays
layout (vertices=4) out;

// varying output to evaluation shader
//out float depth;

uniform mat4 cameraView;
uniform mat4 model;

void main()
{
    // ----------------------------------------------------------------------
    // pass attributes through
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;


    // ----------------------------------------------------------------------
    // invocation zero controls tessellation levels for the entire patch
    int level = 32;
    if (gl_InvocationID == 0)
    {
        const int MIN_TESS_LEVEL = 1;
        const int MAX_TESS_LEVEL = 16;
        const float MIN_DISTANCE = 0;
        const float MAX_DISTANCE = 40;

        // ----------------------------------------------------------------------
        // Step 2: transform each vertex into eye space
        vec4 viewPos00 =  cameraView * gl_in[0].gl_Position;
        vec4 viewPos01 =  cameraView * gl_in[1].gl_Position;
        vec4 viewPos10 =  cameraView * gl_in[2].gl_Position;
        vec4 viewPos11 =  cameraView * gl_in[3].gl_Position;

        // ----------------------------------------------------------------------
        // Step 3: "distance" from camera scaled between 0 and 1
        float distance00 = clamp((abs(viewPos00.z)-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
        float distance01 = clamp((abs(viewPos01.z)-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
        float distance10 = clamp((abs(viewPos10.z)-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
        float distance11 = clamp((abs(viewPos11.z)-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);

        // ----------------------------------------------------------------------
        // Step 4: interpolate edge tessellation level based on closer vertex
        float tessLevel0 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance10, distance00) );
        float tessLevel1 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance00, distance01) );
        float tessLevel2 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance01, distance11) );
        float tessLevel3 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance11, distance10) );

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
        //gl_TessLevelOuter[0] = level;
        //gl_TessLevelOuter[1] = level;
        //gl_TessLevelOuter[2] = level;
        //gl_TessLevelOuter[3] = level;

        //gl_TessLevelInner[0] = level;
        //gl_TessLevelInner[1] = level;
    }
}