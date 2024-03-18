#version 460

layout (vertices=4) out;

// common.tesc stubs
void calculateTessLevels();

void main()
{
    // pass through
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    // invocation 0 controls tessellation levels for the entire patch
    if (gl_InvocationID == 0) calculateTessLevels();
}