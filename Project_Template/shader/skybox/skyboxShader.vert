#version 460

layout (location = 0) in vec3 aPos;

out vec3 vPos;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	vPos = aPos;
	gl_Position = vec4(aPos,1);
}