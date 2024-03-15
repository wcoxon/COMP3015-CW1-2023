#version 460
#define NR_POINT_LIGHTS 2
#define NR_DIR_LIGHTS 1

in vec3 gPos;
in vec3 gNor;
in vec3 faceNor;
in vec2 gTex;
in mat3 TBN;
in vec3 gLight;
in vec3 gColor;

layout (location = 0) out vec4 FragColor;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

uniform sampler2D colourTexture;
uniform sampler2D normalMap;

uniform struct pointLight {
    int lightType;
    int textureID;
    mat4 transform;
    float lightIntensity;
    vec3 lightColour;
    float far_plane;
} lights[NR_POINT_LIGHTS];
uniform samplerCubeArray pointDepthMaps;

uniform struct directionalLight {
    int textureID;
    mat4 transform;
    mat4 project;
    vec3 direction;
    float lightIntensity;
    vec3 lightColour;
    float far_plane;
} directionalLights[NR_DIR_LIGHTS];
uniform sampler2DArray directionalDepthMaps;

uniform struct material {
    float ambientReflectivity;
    float diffuseReflectivity;
    float specularReflectivity;
    int specularPower;

    bool perFragment;
    bool shadeFlat;
} mtl;


vec4 volumetricLight(float stride, vec3 fragPos);

vec3 computeLight(vec3 Pos, vec3 Nor);

void main() 
{
    if(mtl.perFragment){
        
        vec3 normalMapSample = 2*texture(normalMap,gTex).xyz-1;
        vec3 normal = TBN * normalMapSample;

        FragColor = vec4(computeLight(gPos,normal),1)*texture(colourTexture,gTex);

        vec4 volumetric =  volumetricLight(1.5,gPos);
        FragColor *= volumetric.w;
        FragColor += vec4(volumetric.xyz,0);
    }
    else FragColor = vec4(gLight,1)*texture(colourTexture,gTex);
}
