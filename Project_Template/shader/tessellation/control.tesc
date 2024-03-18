#version 460

layout (vertices=4) out;

in vec3 vPos[]; // local
in vec3 vNor[]; // local
in vec2 vTex[];

out vec3 tcPos[]; // local
out vec3 tcNor[]; // local
out vec2 tcTex[];

// common.tesc stubs
float[4] calculateTessLevels();

void main()
{
    // pass attributes through

    tcPos[gl_InvocationID] = vPos[gl_InvocationID];
    tcNor[gl_InvocationID] = vNor[gl_InvocationID];
    tcTex[gl_InvocationID] = vTex[gl_InvocationID];
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    // invocation zero controls tessellation levels for the entire patch
    if (gl_InvocationID == 0)
    {
        gl_TessLevelOuter = calculateTessLevels();

        gl_TessLevelInner[0] = max(gl_TessLevelOuter[1], gl_TessLevelOuter[3]);
        gl_TessLevelInner[1] = max(gl_TessLevelOuter[0], gl_TessLevelOuter[2]);
    }
}