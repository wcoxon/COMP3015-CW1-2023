#version 460

in vec3 gPos;
in vec3 gNor;
in vec2 gTex;
in mat3 TBN;
in vec3 gLight;

out vec4 FragColor;

// uniforms
uniform sampler2D colourTexture;
uniform sampler2D normalMap;

uniform struct material {
    float ambientReflectivity;
    float diffuseReflectivity;
    float specularReflectivity;
    int specularPower;

    bool perFragment;
    bool shadeFlat;
} mtl;

uniform bool gammaCorrection;

// stubs of common.frag
vec4 volumetricLight(float stride, vec3 fragPos);
vec3 computeLight(vec3 Pos, vec3 Nor, vec4 surfaceColour);

void main() 
{
    if(!mtl.perFragment){
        FragColor = vec4(gLight,1)*texture(colourTexture,gTex);
        return;
    }
    
    vec4 colourSample = texture(colourTexture,gTex);

    vec4 normalMapSample = texture(normalMap,gTex)*2-1;
    vec3 normal = normalize(TBN * normalMapSample.xyz);

    FragColor = vec4(computeLight(gPos,normal,colourSample),1);

    if(gammaCorrection){
		// apply gamma correction
		float gamma = 2.2;
		FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
    }

}
