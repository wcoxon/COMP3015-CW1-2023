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

vec4 color = vec4(1);

vec3 viewPos = -(view*vec4(0,0,0,1)).xyz*mat3(view);

vec4 volumetricLight(float stride, vec3 fragPos){
    vec3 viewToFrag = fragPos-viewPos;
    float fragDistance = length(viewToFrag);
    int steps = int(ceil(fragDistance/stride));

    vec3 reflectColour = vec3(1);
    float density = 0.02;
    float scatterPerUnit = 0.001;
    
    directionalLight dirLight = directionalLights[0];
    
    vec4 reflectedLight = vec4(0);

    for(int stepNum = 0; stepNum < steps; stepNum++)
    {
        vec3 samplePosition = viewPos+stepNum*viewToFrag/steps;

        float sampleDistance = stride*stepNum;
        float absorption = exp(-sampleDistance*density);

        vec4 lightSpacePos = dirLight.project * dirLight.transform *vec4(samplePosition.xyz,1);
        
        if(abs(lightSpacePos.x)>lightSpacePos.w || abs(lightSpacePos.y)>lightSpacePos.w ){
            reflectedLight += scatterPerUnit*stepNum*stride*absorption*dirLight.lightIntensity;
            continue;
        }
        
        vec3 depthCoords = ((lightSpacePos.xyz / lightSpacePos.w)+1)/2.0;
        float lightDepth = texture(directionalDepthMaps,depthCoords.xyz).r;

        //if the directional light gets here, light up
        if((depthCoords.z-0.01) < lightDepth){
            reflectedLight += scatterPerUnit*stepNum*stride*absorption*dirLight.lightIntensity;
        }

        for(int pLightIndex = 0; pLightIndex < NR_POINT_LIGHTS; pLightIndex++){
            pointLight pLight = lights[pLightIndex];

            vec3 lightToFrag = (pLight.transform*vec4(samplePosition,1)).xyz;

            if(length(lightToFrag) > pLight.far_plane){
                continue;
            }

            float lightAttenuation = pow(length(lightToFrag),-2);

            float fragDepth = length(lightToFrag);
            
            float lightDepth = texture(pointDepthMaps, vec4(lightToFrag,pLight.textureID)).r * pLight.far_plane;

            //if point light gets here, light up
            if((depthCoords.z-0.01) < lightDepth){
                reflectedLight.rgb += scatterPerUnit*stepNum*stride*absorption*lightAttenuation*pLight.lightIntensity*pLight.lightColour;
            }

        }

    }
    float transmittance = exp(-fragDistance*density);
    float opacity = clamp(transmittance,0,1);
    return vec4(reflectedLight.rgb*reflectColour,opacity);
}

float phongSpecular(vec3 lightDir,vec3 viewDir,vec3 fragNormal){
    vec3 lightReflectDir = lightDir - 2.0*dot(lightDir,fragNormal)*fragNormal;
    return mtl.specularReflectivity * pow(max(-dot(normalize(lightReflectDir),viewDir),0),mtl.specularPower);
}
float blinn_phong(vec3 lightDir, vec3 viewDir, vec3 fragNormal){
    vec3 h = -normalize(lightDir+viewDir);
    return mtl.specularReflectivity*pow(max(dot(h,fragNormal),0),mtl.specularPower);
}

vec3 getViewDirection(){
    return normalize(gPos-viewPos);
}

//Pos (world position), Nor (world normal)
vec3 computeLight(vec3 Pos, vec3 Nor, vec4 surfaceColour){

    vec3 worldNor = Nor;//normalize(transpose(inverse(mat3(model)))*Nor);


    vec3 viewToFrag = Pos-viewPos;

    vec3 Light = vec3(0);

    float Ambient = mtl.ambientReflectivity;

    //point lights
    for(int lightIndex = 0; lightIndex<NR_POINT_LIGHTS;lightIndex++){

        vec3 lightToFrag = (lights[lightIndex].transform*vec4(Pos,1)).xyz;

        float lightAttenuation =1/pow(length(lightToFrag),2);

        //depth of the fragment from light
        float fragDepth = length(lightToFrag);
        //depth of the light in this direction
        float lightDepth = texture(pointDepthMaps,vec4(lightToFrag,lights[lightIndex].textureID)).r * lights[lightIndex].far_plane;
        
        float bias = 0.01;

        if((fragDepth -  bias) < lightDepth){
            float diffuse = mtl.diffuseReflectivity * max(-dot(normalize(lightToFrag),worldNor),0);
            float specular = blinn_phong(normalize(lightToFrag),normalize(viewToFrag),worldNor);

            Light += lights[lightIndex].lightColour*lights[lightIndex].lightIntensity*(Ambient+diffuse + specular)*lightAttenuation;
        }
        else{
            Light+=lights[lightIndex].lightColour*lights[lightIndex].lightIntensity*Ambient*lightAttenuation;
        }
    }

    //directional lighting
    for(int i = 0;i<NR_DIR_LIGHTS;i++){
        //get this light's info
        directionalLight light = directionalLights[i];
        vec3 lightDir = -normalize(vec3(light.transform[0][2], light.transform[1][2], light.transform[2][2]));

        // calculate whether light hits fragment

        vec4 lightSpacePos = light.project* light.transform *vec4(Pos,1);

        //vec3 lightFragPos = lightSpacePos.xyz / lightSpacePos.w;

        vec3 depthCoords = (lightSpacePos.xyz / lightSpacePos.w + 1)/2;

        //float fragDepth = (lightFragPos.z+1)/2;
        
        //vec3 depthMapTexCoords = vec3(depthCoords,i);
        float lightDepth = texture(directionalDepthMaps,depthCoords).r;
        
        float bias = 0.01;

        if((depthCoords.z -  bias) < lightDepth){
            //calculate reflections
            float Diffuse = mtl.diffuseReflectivity * max(-dot(normalize(lightDir),worldNor),0);
            float specular = blinn_phong(lightDir,normalize(viewToFrag),worldNor);
            
            Light +=  light.lightIntensity*(Ambient + Diffuse + specular) * light.lightColour;
        }
        else{
            Light+=light.lightColour*light.lightIntensity*Ambient;
        }
     }

     Light *= surfaceColour.rgb;

     vec4 volumetric = volumetricLight(1.5f, Pos);
     Light *= volumetric.w;
     Light += volumetric.rgb;

     return clamp(Light,0,1);
}