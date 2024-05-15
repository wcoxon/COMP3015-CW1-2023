#version 460
in float dist;

void main()
{
    gl_FragDepth = dist;
}  