#version 460

in vec3 gPos; // world
in vec3 gNor; // world
in vec2 gTex;
in vec3 gLight;

out vec4 FragColor;

// uniforms
uniform vec3 boatPosition;
uniform float boatSpeed;

uniform float time;
uniform float waveSpeed;

uniform sampler2D colourTexture;
uniform sampler2D normalMap;
uniform samplerCube skybox;

layout(binding = 5) uniform sampler2D foamTexture;

uniform struct material {
    float ambientReflectivity;
    float diffuseReflectivity;
    float specularReflectivity;
    int specularPower;

    bool perFragment;
    bool shadeFlat;
} mtl;


// common.frag stubs
vec3 viewPos;
float diffuse;
float specular;
vec3 computeLight(vec3 Pos, vec3 Nor, vec4 surfaceColour);


void main()
{
    if(!mtl.perFragment){
        FragColor = vec4(gLight,1)*texture(colourTexture,gTex);
        return;
    }

    vec2 worldTextureCoords = gPos.xz/30.0f;
    
    //foam and water blending
    vec4 waterColour = texture(colourTexture,worldTextureCoords);
    vec4 foamColour = texture(foamTexture,worldTextureCoords);

    float foamStart = boatSpeed*0.2; 
    float foamEnd = boatSpeed*0.4;
    float foamFade = foamEnd-foamStart;

    float mixFactor = clamp((distance(gPos, boatPosition) - foamStart)/foamFade,0,1);

    vec4 mixColour = mix(foamColour,waterColour,mixFactor);

    diffuse = mix(1,mtl.diffuseReflectivity,mixFactor);
    specular = mix(0,mtl.specularReflectivity,mixFactor);

    //normal mapping
    vec2 rippleDisplacement0 = vec2(1,0)*time/30.f;
    vec2 rippleDisplacement1 = vec2(0,1)*time/30.f;

    vec3 normalMapSamples[2] = {
        texture(normalMap, worldTextureCoords+rippleDisplacement0).xyz*2 - 1,
        texture(normalMap, worldTextureCoords+rippleDisplacement1).xyz*2 - 1
    };


    
    vec3 normal = normalize(gNor);

    vec3 T = cross(normal,vec3(0,0,-1));
    vec3 B = cross(normal,T);
    mat3 TBN = mat3(T,B,normal);
    normal = TBN * normalMapSamples[0];

    normal = normalize(normal);

    T = cross(normal,vec3(0,0,1));
    B = cross(normal,T);
    TBN = mat3(T,B,normal);
    normal = TBN * normalMapSamples[1];

    normal = normalize(normal);

    //vec3 viewToFrag = gPos-viewPos;
    //vec4 skyboxReflection = texture(skybox,reflect(viewToFrag,normal));

    //FragColor = skyboxReflection;
    //FragColor =  vec4(normal,1);
    FragColor =  vec4(computeLight(gPos,normal,mixColour),1);

}
