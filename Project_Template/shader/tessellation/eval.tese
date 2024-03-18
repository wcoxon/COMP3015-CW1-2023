#version 460

layout (quads, equal_spacing, ccw) in;

// received from Tessellation Control Shader - all texture coordinates for the patch vertices
in vec3 tcPos[]; //local
in vec3 tcNor[]; //local
in vec2 tcTex[];

out vec3 vPos; //world
out vec3 vNor; //local
out vec2 vTex;

// uniforms
uniform mat4 model;           // the model matrix
uniform mat4 view;            // the view matrix
uniform mat4 projection;      // the projection matrix

uniform float time;

uniform float wavelength; //unit length of wave
uniform float waveSpeed; //units per second wave speed
uniform float amplitude; //wave height

void main()
{
    // get patch coordinate
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;


    // retrieve control point texture coordinates
    vec2 t00 = tcTex[0];
    vec2 t01 = tcTex[1];
    vec2 t10 = tcTex[2];
    vec2 t11 = tcTex[3];
    // bilinearly interpolate texture coordinate across patch
    vec2 t0 = (t01 - t00) * u + t00;
    vec2 t1 = (t11 - t10) * u + t10;
    vec2 texCoord = (t1 - t0) * v + t0;


    // retrieve control point positions (local)
    vec3 pos00 = tcPos[0];
    vec3 pos01 = tcPos[1];
    vec3 pos10 = tcPos[2];
    vec3 pos11 = tcPos[3];
    // bilinearly interpolate pos across patch
    vec3 pos0 = (pos01-pos00)*u + pos00;
    vec3 pos1 = (pos11-pos10)*u + pos10;
    vec3 pos = (pos1 - pos0)*v + pos0;


    // retrieve control point position coordinates (world)
    vec4 position00 = gl_in[0].gl_Position;
    vec4 position01 = gl_in[1].gl_Position;
    vec4 position10 = gl_in[2].gl_Position;
    vec4 position11 = gl_in[3].gl_Position;
    // bilinearly interpolate position coordinate across patch
    vec4 position0 = (position01 - position00) * u + position00;
    vec4 position1 = (position11 - position10) * u + position10;
    vec4 position = (position1 - position0) * v + position0;

    // compute patch surface normal
    //vec4 uVec = p01 - p00;
    //vec4 vVec = p10 - p00;
    //vec4 normal = normalize( vec4(cross(vVec.xyz, uVec.xyz), 0) );

    //wave stuff
    //vec4 worldPos = position;//model*vec4(pos,1);

    vec3 waveDisplacement = vec3(0, amplitude*sin(position.x/wavelength+time*waveSpeed), 0);
    vec3 waveNormal = vec3((amplitude/wavelength)*-cos(position.x/wavelength+time*waveSpeed), 1, 0);

    position += vec4(waveDisplacement,0);

    vPos = position.xyz;//worldPos.xyz;
    vNor = waveNormal;
    vTex = texCoord;


    // output patch point position in clip space
    gl_Position = projection*view*position;
}