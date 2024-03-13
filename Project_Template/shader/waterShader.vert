#version 460

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNor;
layout (location = 3) in vec2 aTex;

out vec3 vPos;
out vec3 vNor;
out vec2 vTex;
out vec3 vColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float time;

//unit length of wave
float wavelength = 2;
//units per second
float speed = 2;
//vertical scale
float amplitude = 1;


void main()
{
    //vec3 waveDisplacement = vec3(0, scale*sin(aPos.x*waves+time*speed), 0);
    //vec3 waveNormal = vec3(scale*waves*-cos(aPos.x*waves+time*speed), 1, 0);
    
    //vPos = aPos+waveDisplacement;
    
    //gonna try sending world position through instead, since when do we ever need local position anyway
    //this will allow us to apply displacements in here without having to store it in local space
    //future shaders do model*Pos, so putting displacement in Pos means it's scaled from local
    //but if we do the model part in here instead, we can apply the displacement afterwards, also less calculations later
    
    vPos = (model*vec4(aPos,1)).xyz;

    vec3 waveDisplacement = vec3(0, amplitude*sin(vPos.x/wavelength+time*speed), 0);

    vPos += waveDisplacement;


    vec3 waveNormal = vec3((amplitude/wavelength)*-cos(vPos.x/wavelength+time*speed), 1, 0);

    vNor = normalize(waveNormal);
    vTex = aTex;
    vColor = vec3(1);
    
    gl_Position = projection*view*vec4(vPos,1);//model*vec4(vPos,1);
}