#version 460

layout (vertices=4) out;

in vec3 vNor[]; // local
in vec2 vTex[];

out vec3 tcNor[]; // local
out vec2 tcTex[];

// common.tesc stubs
void calculateTessLevels();

void main()
{
    // pass attributes through
    tcNor[gl_InvocationID] = vNor[gl_InvocationID];
    tcTex[gl_InvocationID] = vTex[gl_InvocationID];
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    // invocation zero controls tessellation levels for the entire patch
    if (gl_InvocationID == 0) calculateTessLevels();
}