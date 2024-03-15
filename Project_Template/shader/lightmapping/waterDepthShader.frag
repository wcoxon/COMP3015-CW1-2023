#version 460
//in vec4 FragPos;
//in vec3 lightPos;

in float dist;
//uniform float far_plane;

void main()
{
    gl_FragDepth = dist;
}  