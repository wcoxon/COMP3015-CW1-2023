#version 460

#define NR_POINT_LIGHTS 2
#define NR_DIR_LIGHTS 1

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 vPos[3]; // world
in vec3 vNor[3]; // local
in vec2 vTex[3];

out vec3 gPos; // world
out vec3 gNor; // world
out vec2 gTex;
out vec3 gLight;
out mat3 TBN;

// uniforms
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

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


float blinn_phong(vec3 lightDir, vec3 viewDir, vec3 Normal){
    vec3 h = -normalize(lightDir+viewDir);
    return mtl.specularReflectivity*pow(max(dot(h,Normal),0),mtl.specularPower);
}

//position (world), normal(world)
vec3 gouraudLighting(vec3 Pos, vec3 Nor){

            vec3 worldVertexPos = Pos;
            vec3 worldVertexNor = Nor;//transpose(inverse(mat3(model)))*Nor;

            
            vec3 worldViewPos = -(view*vec4(0,0,0,1)).xyz*mat3(view);
            vec3 viewDir = normalize(worldVertexPos-worldViewPos);

            vec3 Light = vec3(0);
            
            float Ambient = mtl.ambientReflectivity;

            for(int lightIndex = 0; lightIndex < NR_POINT_LIGHTS; lightIndex++){
                vec3 lightDir = normalize((lights[lightIndex].transform*vec4(worldVertexPos,1.0)).xyz);

                vec3 lightReflectDir = lightDir - 2.0*dot(lightDir,worldVertexNor)*worldVertexNor;

                float lightAttenuation = 1/pow(length((lights[lightIndex].transform*vec4(worldVertexPos,1.0)).xyz),2);

                float Diffuse = mtl.diffuseReflectivity * max(-dot(lightDir, worldVertexNor),0);
                float Specular = mtl.specularReflectivity * pow(max(-dot(lightReflectDir,viewDir),0),mtl.specularPower);
                
                vec3 fragToLight = (lights[lightIndex].transform*vec4(worldVertexPos,1.0)).xyz;

                float fragDepth = length(fragToLight);

        
                float lightDepth = texture(pointDepthMaps,vec4(fragToLight,lightIndex)).r*lights[lightIndex].far_plane;
        
                float bias = 0.05;

                if((fragDepth -  bias) < lightDepth){
                    Light+= lights[lightIndex].lightIntensity*lightAttenuation*(Ambient + Diffuse + Specular)*lights[lightIndex].lightColour;
                }
            }

            for(int i = 0;i<NR_DIR_LIGHTS;i++){
                //get this light's info
                directionalLight light = directionalLights[i];
                vec3 lightDir = -normalize(vec3(light.transform[0][2], light.transform[1][2], light.transform[2][2]));

                // calculate whether light hits fragment

                vec4 lightSpacePos = light.project* light.transform *vec4(worldVertexPos.xyz,1);

                vec3 lightFragPos = lightSpacePos.xyz / lightSpacePos.w;

                float fragDepth = (lightFragPos.z+1)/2;
        
                vec3 depthMapTexCoords = vec3((lightFragPos.xy+1)/2,i);
                float lightDepth = texture(directionalDepthMaps,depthMapTexCoords).r;
        
                float bias = 0.05;

                if((fragDepth -  bias) < lightDepth){
                    //calculate reflections
                    float Diffuse = mtl.diffuseReflectivity * max(-dot(normalize(lightDir),worldVertexNor),0);
                    float specular = blinn_phong(lightDir,viewDir,worldVertexNor);
            
                    Light +=  light.lightIntensity*(Ambient + Diffuse + specular) * light.lightColour;
                }
            }
            return clamp(Light,0,1);
         }


void main() {

    //get the change in position over each edge (world)
    vec3 edge1 = vPos[1] - vPos[0];
    vec3 edge2 = vPos[2] - vPos[0];
    //get the change in texture coordinate over each edge
    vec2 deltaUV1 = vTex[1] - vTex[0];
    vec2 deltaUV2 = vTex[2] - vTex[0];

    // normal vector
    vec3 faceNormal = normalize(cross(edge1,edge2));
    
    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    vec3 faceTangent;
    faceTangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    faceTangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    faceTangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    faceTangent = normalize(faceTangent);

    for(int x = 0; x<3;x++){

        gPos = vPos[x];
        gTex = vTex[x];
        gl_Position = gl_in[x].gl_Position;

        // use face normal or individuals
        if(mtl.shadeFlat) {
            gNor = faceNormal;

            vec3 faceBitangent = normalize(cross(faceNormal,faceTangent));

            TBN = mat3(faceTangent,faceBitangent,faceNormal); //face tbn
        }
        else{
            gNor = normalize((model*vec4(vNor[x],0)).xyz);

            vec3 vertTangent = normalize(faceTangent - dot(faceTangent, gNor) * gNor); //orthogonalize tangent
            vec3 vertBitangent = normalize(cross(gNor,vertTangent));

            TBN = mat3(vertTangent,vertBitangent,gNor); //vertex tbn
        }

        // gouraud lighting computation
        if(!mtl.perFragment) gLight = gouraudLighting(gPos,gNor);
        
        EmitVertex();
    }
    EndPrimitive();
}  