#version 460

// position (clip space)
layout (location = 0) in vec3 aPos;

out vec3 sampleDirection;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	vec4 clipPos = vec4(aPos,1);

	vec4 viewSpacePos = inverse(projection)*clipPos;
	
	vec4 worldDirection = inverse(view)*vec4(viewSpacePos.xyz,0);
    
	sampleDirection =  worldDirection.xyz;

	gl_Position = vec4(aPos,1);
}