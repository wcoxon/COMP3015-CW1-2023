#version 460

layout (location = 0) in vec3 aPos;

out vec3 vPos;

void main()
{
	vPos = (aPos+1.0f)/2.0f;

	gl_Position = vec4(aPos,1);
}