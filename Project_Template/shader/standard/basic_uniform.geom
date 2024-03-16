#version 460
#define NR_LIGHTS 2
#define NR_DIR_LIGHTS 1

layout (triangles) in;

//positions (local)
in vec3 vPos[3];
in vec3 vNor[3];
in vec2 vTex[3];
in vec3 vColor[3];

layout (triangle_strip, max_vertices = 3) out;

//position (local)
out vec3 gPos;
out vec3 gNor;
out vec2 gTex;
out vec3 gColor;
out vec3 gLight;
out mat3 TBN;

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
} lights[NR_LIGHTS];
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

vec3 gouraudLighting(vec3 Pos, vec3 Nor){
//careful don't leave this like this if you're reworking input pos to worldspace
            vec3 worldVertexPos = (model*vec4(Pos,1)).xyz;
            vec3 worldVertexNor = transpose(inverse(mat3(model)))*Nor;//localVertexNor;

            
            vec3 worldViewPos = -(view*vec4(0,0,0,1)).xyz*mat3(view);
            vec3 viewDir = normalize(worldVertexPos-worldViewPos);//normalize(vPos[x]-viewPosition);

            vec3 Light = vec3(0);

            
            float Ambient = mtl.ambientReflectivity;

            for(int lightIndex = 0; lightIndex < NR_LIGHTS; lightIndex++){
                vec3 lightDir = normalize((lights[lightIndex].transform*vec4(worldVertexPos,1.0)).xyz);

                vec3 lightReflectDir = lightDir - 2.0*dot(lightDir,worldVertexNor)*worldVertexNor;

                float lightAttenuation = 1/pow(length((lights[lightIndex].transform*vec4(worldVertexPos,1.0)).xyz),2);

                float Diffuse = mtl.diffuseReflectivity * max(-dot(lightDir, worldVertexNor),0);
                float Specular = mtl.specularReflectivity * pow(max(-dot(lightReflectDir,viewDir),0),mtl.specularPower);
                
                //Light += lights[lightIndex].lightIntensity*lightAttenuation*(Ambient + Diffuse + Specular)*lights[lightIndex].lightColour;

                vec3 fragToLight = (lights[lightIndex].transform*vec4(worldVertexPos,1.0)).xyz;

                float fragDepth = length(fragToLight);

        
                float lightDepth = texture(pointDepthMaps,vec4(fragToLight,lightIndex)).r*lights[lightIndex].far_plane;
        
                float bias = 0.05;
                //shadow = (fragDepth -  bias) < lightDepth ? 1.0 : 0.0;

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
    gColor = vColor[0];


    // positions (world positions, so the calculated normal will be in world too, maybe world won't work idk)
    vec3 pos1 = vPos[0];
    vec3 pos2= vPos[1];
    vec3 pos3= vPos[2];
    // texture coordinates
    vec2 uv1 = vTex[0];
    vec2 uv2 = vTex[1];
    vec2 uv3 = vTex[2];

    //get the change in position over each edge
    vec3 edge1 = pos2 - pos1;
    vec3 edge2 = pos3 - pos1;
    //get the change in texture coordinate over each edge
    vec2 deltaUV1 = uv2 - uv1;
    vec2 deltaUV2 = uv3 - uv1;


    // normal vector
    vec3 faceNormal = normalize(cross(edge1,edge2));

    vec3 tangent;
    vec3 bitangent;

    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
    
    tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

    //this fixes it?? ergh
    bitangent = cross(tangent,faceNormal);

    tangent = normalize(tangent);
    bitangent = normalize(bitangent);

    TBN = mat3(tangent,bitangent,faceNormal);

    for(int x = 0; x<3;x++){

        gPos = vPos[x];
        gTex = vTex[x];

        // use face normal or individuals
        if(mtl.shadeFlat) gNor = faceNormal;
        else gNor = vNor[x];

        // per-vertex lighting computation
        if(!mtl.perFragment){
            gLight = gouraudLighting(gPos,gNor);
        }
        
        gl_Position = gl_in[x].gl_Position;

        EmitVertex();
    }
    
    EndPrimitive();

}  