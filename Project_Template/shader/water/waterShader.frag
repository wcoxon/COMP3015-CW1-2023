#version 460
#define NR_POINT_LIGHTS 2
#define NR_DIR_LIGHTS 1

//subroutine void RenderPassType();
//subroutine uniform RenderPassType RenderPass;

in vec3 gPos;
in vec3 gNor;
in vec3 faceNor;
in vec2 gTex;
in vec3 gLight;
in vec3 gColor;

layout (location = 0) out vec4 FragColor;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

uniform float time;

uniform sampler2D colourTexture;
uniform sampler2D normalMap;
layout(binding = 4) uniform samplerCube skybox;

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

vec3 computeLight(vec3 Pos, vec3 Nor, vec4 surfaceColour);

vec3 getViewDirection();

vec3 viewPos;

void main() 
{
    if(mtl.perFragment) {
        vec2 waterV1 = vec2(-0.05,-0.02);
        vec2 waterV2 = vec2(-0.01,0.05);

        vec3 normalMapSamples[2] = {
            texture(normalMap,gPos.xz/20.0f+waterV1*time).xyz*2.0-1.0,
            texture(normalMap,gPos.xz/20.0f+waterV2*time).xyz*2.0-1.0
        };
    
        vec3 T = cross(gNor,vec3(0,0,1));
        vec3 B = cross(gNor,T);
        mat3 TBN = mat3(T,B,gNor);
        vec3 normal = TBN* normalMapSamples[0];
    
        T = cross(normal,vec3(0,0,1));
        B = cross(normal,T);
        TBN = mat3(T,B,normal);
        normal = TBN * normalMapSamples[1];

        normal = normalize(normal);

        //vec3 worldNor = normalize(transpose(inverse(mat3(model)))*gNor);
        //vec3 viewDir = normalize(gPos-viewPos);

        FragColor =  vec4(computeLight(gPos,normal,texture(colourTexture,gTex)),1);//*texture(colourTexture,gTex);// texture(skybox,reflect(viewDir,worldNor));

    }
    else FragColor = vec4(gLight,1)*texture(colourTexture,gTex);
}
