#version 460
#define NR_LIGHTS 2
#define NR_DIR_LIGHTS 1

layout (triangles) in;

in vec3 vPos[3];
in vec3 vNor[3];
in vec2 vTex[3];
in vec3 vColor[3];

layout (triangle_strip, max_vertices = 3) out;

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


void main() {
    gColor = vColor[0];

    vec3 localViewPos = vec3(0,0,0);
    vec3 worldViewPos = -(view*vec4(0,0,0,1)).xyz*mat3(view);
    
    for(int x = 0; x<3;x++){

        gPos = vPos[x];
        gTex = vTex[x];

        if(mtl.shadeFlat) {
            //flat shading calculates face normal and assigns to all vertices
            gNor = normalize(cross(vPos[1]-vPos[0],vPos[2]-vPos[0]));

        }
        else gNor = vNor[x];

        if(gNor.y!=1){
            //swivel tangent space from y axis
            vec3 tangent = cross(vec3(0,1,0),gNor);
            //vec3 tangent = vec3(-gNor.y,gNor.x,gNor.z);
            vec3 bitangent = cross(gNor,tangent);
            vec3 normal = gNor;
            TBN = mat3(tangent,bitangent,normal);
        }
        else{
            vec3 tangent = cross(vec3(0,0,1),gNor);
            //vec3 tangent = vec3(-gNor.y,gNor.x,gNor.z);
            vec3 bitangent = cross(gNor,tangent);
            vec3 normal = gNor;
            TBN = mat3(tangent,bitangent,normal);
        }

        //per-vertex lighting computation
        if(!mtl.perFragment){

        //careful don't leave this like this if you're reworking input pos to worldspace
            vec3 worldVertexPos = (model*vec4(vPos[x],1)).xyz;
            vec3 worldVertexNor = transpose(inverse(mat3(model)))*gNor;//localVertexNor;

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

            gLight = clamp(Light,0,1);
        }
        
        gl_Position = gl_in[x].gl_Position;

        EmitVertex();
    }
    
    EndPrimitive();

}  