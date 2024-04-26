#version 460

layout (location = 0) in vec3 vPos;

out vec4 colour;

uniform mat4 view;
uniform mat4 projection;

uniform samplerCube skybox;

uniform struct directionalLight {
    int textureID;
    mat4 transform;
    mat4 project;
    vec3 direction;
    float lightIntensity;
    vec3 lightColour;
    float far_plane;
} directionalLights[1];
uniform sampler2DArray directionalDepthMaps;

vec3 viewPos = -(view*vec4(0,0,0,1)).xyz*mat3(view);

vec4 volumetricLight(float stride, vec3 fragPos){
    vec3 viewToFrag = fragPos-viewPos;
    float fragDistance = length(viewToFrag);
    int steps = int(ceil(fragDistance/stride));

    vec3 reflectColour = vec3(1);
    float density = 0.04;
	float scatterPerUnit = 0.001;
    
    directionalLight dirLight = directionalLights[0];
    
    float reflectedLight = 0;

    for(int stepNum = 0; stepNum < steps; stepNum++)
    {
        vec3 samplePosition = viewPos+stepNum*viewToFrag/steps;

        float sampleDistance = stride*stepNum;
        float absorption = exp(-sampleDistance*density);

        vec4 lightSpacePos = dirLight.project * dirLight.transform *vec4(samplePosition.xyz,1);
        
        if(abs(lightSpacePos.x)>lightSpacePos.w || abs(lightSpacePos.y)>lightSpacePos.w ||  lightSpacePos.z>60){
            reflectedLight += scatterPerUnit*stride*absorption;
            continue;
        }
        
        vec3 depthCoords = ((lightSpacePos.xyz / lightSpacePos.w)+1)/2.0;
        float lightDepth = texture(directionalDepthMaps,depthCoords.xyz).r;

        //if the directional light gets here
        if((depthCoords.z-0.01) < lightDepth){
            reflectedLight += scatterPerUnit*stride*absorption;
        }
    }
    float transmittance = exp(-fragDistance*density);
    float opacity = clamp(transmittance,0,1); //clamp(dist*absorbPerUnit,0,1);
    return vec4(clamp(reflectedLight,0,1)*reflectColour,opacity);
}

void main()
{
	//vec3 fragDirection = normalize(inverse(mat3(view))*vPos);
    //vec4 fragClipSpace = vec4(vPos,1);

    //vec4 fragViewSpace = inverse(projection) * fragClipSpace;
    //vec4 fragWorldSpace =  inverse(view) * fragViewSpace;

    vec4 fragWorldSpace =  inverse(view) * inverse(projection) * vec4(vPos.xyz,1);


    //vec3 fragWorldPos = viewPos+fragDirection*50;
    //colour = vec4(fragmentWorldSpace.xyz/50,1);
    colour = vec4(volumetricLight(1.6,fragWorldSpace.xyz*50).xyz,1);
    

	gl_FragDepth = 1;
}