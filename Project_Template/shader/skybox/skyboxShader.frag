#version 460

layout (location = 0) in vec3 sampleDirection;

out vec4 colour;

layout(binding = 4) uniform samplerCube skybox;
uniform bool gammaCorrection = false;
uniform float gamma = 1;

void main()
{
    colour = texture(skybox,sampleDirection);

	if(gammaCorrection){
		colour.rgb = pow(colour.rgb, vec3(1.0/gamma));
    }

	gl_FragDepth = 1;
}