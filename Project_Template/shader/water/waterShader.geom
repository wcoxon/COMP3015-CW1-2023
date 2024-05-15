#version 460
#define NR_LIGHTS 2
#define NR_DIR_LIGHTS 1

layout (triangles) in;
in vec3 vPos[3];
in vec3 vNor[3];
in vec2 vTex[3];

layout (triangle_strip, max_vertices = 3) out;
out vec3 gPos;
out vec3 gNor;
out vec2 gTex;
out vec3 gLight;
out mat3 TBN;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform float time;

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

    vec3 localViewPos = vec3(0,0,0);
    vec3 worldViewPos = -(view*vec4(0,0,0,1)).xyz*mat3(view);
    
    for(int x = 0; x<3;x++){

        gPos = (model*vec4(vPos[x],1)).xyz;

        gTex = vTex[x];


        if(mtl.shadeFlat) {
            //flat shading calculates face normal and assigns to all vertices
            gNor = normalize(cross(vPos[1]-vPos[0],vPos[2]-vPos[0]));

        }
        else gNor = vNor[x]; 

        if(gNor.y!=1){
            //swivel tangent space from y axis
            vec3 tangent = cross(vec3(0,1,0),gNor);
            vec3 bitangent = cross(gNor,tangent);
            vec3 normal = gNor;
            TBN = mat3(tangent,bitangent,normal);
        }
        else{
            vec3 tangent = cross(vec3(0,0,1),gNor);
            vec3 bitangent = cross(gNor,tangent);
            vec3 normal = gNor;
            TBN = mat3(tangent,bitangent,normal);
        }
        

        gl_Position = gl_in[x].gl_Position;

        EmitVertex();
    }
    
    EndPrimitive();

}  