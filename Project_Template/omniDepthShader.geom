#version 460
//takes primitives of a model (ie triangles), for each light the primitive is rendered to the cubemap faces
#define LIGHT_NR 2
layout (triangles) in;
layout (triangle_strip, max_vertices=18*LIGHT_NR) out;

//the views of the cubemap

mat3 directionTransforms[6] = {
    {{0,0,-1},{0,-1,0},{-1,0,0}},
    {{0,0,1},{0,-1,0},{1,0,0}}, 
    {{1,0,0},{0,0,-1},{0,1,0}}, 
    {{1,0,0},{0,0,1},{0,-1,0}}, 
    {{1,0,0},{0,-1,0},{0,0,-1}}, 
    {{-1,0,0},{0,-1,0},{0,0,1}}, 
};

uniform float far_plane;

uniform struct PointLight{
    mat4 project;
    mat4 transform;
} lights[LIGHT_NR];

//out vec3 lightPos;
out float dist;
out vec4 FragPos; // FragPos from GS (output per emitvertex)
//vec4 FragPos;
void main()
{
    
    for(int lightIndex = 0; lightIndex < LIGHT_NR; lightIndex++){
        for(int faceIndex = 0; faceIndex < 6; faceIndex++)
        {
            gl_Layer =lightIndex*6+faceIndex;
            
            for(int vertex = 0; vertex < 3; vertex++) // for each triangle vertex
            {
                // (gl_position is model*vertexposition i.e. world position)
                FragPos = gl_in[vertex].gl_Position;
                

                gl_Position = 
                    lights[lightIndex].project*
                    mat4(directionTransforms[faceIndex])*
                    lights[lightIndex].transform*
                    FragPos;

                
                dist=length(gl_Position.xyz)/far_plane;
                EmitVertex();
            }    
            EndPrimitive();
        }
    }
}  