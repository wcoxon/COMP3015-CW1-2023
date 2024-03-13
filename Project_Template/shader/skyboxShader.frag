#version 460

layout (location = 0) in vec3 vPos;

out vec4 colour;


uniform mat4 view;
uniform mat4 projection;

uniform samplerCube skybox;

void main()
{
	vec3 fragDirection = normalize(inverse(mat3(view))*vPos);
    
    colour = texture(skybox,fragDirection);

	gl_FragDepth = 1;
}