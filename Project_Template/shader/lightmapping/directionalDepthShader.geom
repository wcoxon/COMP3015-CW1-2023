#version 460
//takes primitives of a model (ie triangles), for each light the primitive is rendered to the cubemap faces
#define LIGHT_NR 1
layout (triangles) in;
layout (triangle_strip, max_vertices=3*LIGHT_NR) out;

uniform float far_plane;

uniform struct DirectionalLight{
    mat4 transform;
    mat4 project;
} lights[LIGHT_NR];

out float dist;

void main()
{
    for(int lightIndex = 0; lightIndex < LIGHT_NR; lightIndex++){
        gl_Layer = lightIndex;
        for(int vertex = 0; vertex < 3; vertex++) // for each triangle vertex
        {
            gl_Position = lights[lightIndex].project*lights[lightIndex].transform*gl_in[vertex].gl_Position;

            dist = (gl_Position.z+1)/2;
            EmitVertex();
        } 
        EndPrimitive();
    }
}  