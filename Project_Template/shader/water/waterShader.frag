#version 460
#define NR_POINT_LIGHTS 2
#define NR_DIR_LIGHTS 1

//subroutine void RenderPassType();
//subroutine uniform RenderPassType RenderPass;

in vec3 gPos; //world
in vec3 gNor; //world
in vec2 gTex;
in vec3 gLight;
in vec3 gColor;

layout (location = 0) out vec4 FragColor;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

uniform vec3 boatPosition;


uniform float time;

uniform sampler2D colourTexture;
uniform sampler2D normalMap;
uniform samplerCubeArray pointDepthMaps;
uniform sampler2DArray directionalDepthMaps;
layout(binding = 4) uniform samplerCube skybox;

uniform struct pointLight {
    int lightType;
    int textureID;
    mat4 transform;
    float lightIntensity;
    vec3 lightColour;
    float far_plane;
} lights[NR_POINT_LIGHTS];

uniform struct directionalLight {
    int textureID;
    mat4 transform;
    mat4 project;
    vec3 direction;
    float lightIntensity;
    vec3 lightColour;
    float far_plane;
} directionalLights[NR_DIR_LIGHTS];

uniform struct material {
    float ambientReflectivity;
    float diffuseReflectivity;
    float specularReflectivity;
    int specularPower;

    bool perFragment;
    bool shadeFlat;
} mtl;

vec4 volumetricLight(float stride, vec3 fragPos);

vec3 computeLight(vec3 Pos, vec3 Nor, vec4 surfaceColour);

vec3 getViewDirection();

vec3 viewPos;

float diffuse;

void main() 
{
    if(mtl.perFragment) {
        vec2 waterV1 = vec2(-0.05,-0.02);
        vec2 waterV2 = vec2(-0.01,0.05);

        vec3 normalMapSamples[2] = {
            texture(normalMap,gPos.xz/30.0f+waterV1*time).xyz*2.0-1.0,
            texture(normalMap,gPos.xz/30.0f+waterV2*time).xyz*2.0-1.0
        };
    
        vec3 T = normalize(cross(gNor,vec3(0,0,1)));
        vec3 B = normalize(cross(gNor,T));
        mat3 TBN = mat3(T,B,gNor);
        vec3 normal = normalize(TBN* normalMapSamples[0]);
    
        T = normalize(cross(normal,vec3(0,0,1)));
        B = normalize(cross(normal,T));
        TBN = mat3(T,B,normal);
        normal = TBN * normalMapSamples[1];

        normal = normalize(normal);


        vec4 waterColour = texture(colourTexture,gTex);
        vec4 foamColour = vec4(1);

        float foamStart = 5;
        float foamEnd = 6;
        float foamFade = foamEnd-foamStart;

        float mixFactor = clamp((distance(gPos, boatPosition) - foamStart)/foamFade,0,1);

        vec4 mixColour = mix(foamColour,waterColour,mixFactor);

        diffuse = mix(1,mtl.diffuseReflectivity,mixFactor);

        vec3 mixNormal = normalize(mix(gNor,normal,mixFactor));


        FragColor =  vec4(computeLight(gPos,mixNormal,mixColour),1);

    }
    else FragColor = vec4(gLight,1)*texture(colourTexture,gTex);
}
