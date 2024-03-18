#version 460

layout (vertices=4) out;

// uniforms
uniform mat4 cameraView;
uniform mat4 model;

uniform vec3 boatPosition;

// common.tesc stubs
float[4] calculateTessLevels();

void main()
{
    // pass through
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    // invocation 0 controls tessellation levels for the entire patch
    if (gl_InvocationID == 0)
    {
        gl_TessLevelOuter = calculateTessLevels();

        gl_TessLevelInner[0] = max(gl_TessLevelOuter[1], gl_TessLevelOuter[3]);
        gl_TessLevelInner[1] = max(gl_TessLevelOuter[0], gl_TessLevelOuter[2]);
    }
}