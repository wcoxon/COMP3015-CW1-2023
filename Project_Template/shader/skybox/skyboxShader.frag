#version 460

layout (location = 0) in vec3 vPos;

out vec4 colour;


uniform mat4 view;
uniform mat4 projection;

layout(binding = 4) uniform samplerCube skybox;

uniform bool gammaCorrection = false;

void main()
{
	//apply camera rotation to forward vertex position

	vec4 clipPos = vec4(vPos,1); // concentric frag dimensions

	vec4 viewSpacePos = inverse(projection)*clipPos; // spread directions with frustum
	
	vec4 worldDirection = inverse(view)*vec4(viewSpacePos.xyz,0); // rotated directions to view angle
    


	vec3 fragDirection = normalize(worldDirection.xyz);

    colour = texture(skybox,fragDirection);

	if(gammaCorrection){
		// apply gamma correction
		float gamma = 2.2;
		colour.rgb = pow(colour.rgb, vec3(1.0/gamma));
    }

	gl_FragDepth = 1;
}