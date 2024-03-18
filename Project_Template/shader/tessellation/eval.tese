#version 460

in vec3 tcNor[]; //local
in vec2 tcTex[];

out vec3 vPos; //world
out vec3 vNor; //local
out vec2 vTex;

// uniforms
uniform mat4 projection;
uniform mat4 view;

uniform float time;
uniform float wavelength;
uniform float waveSpeed;
uniform float amplitude;

// common.tese stubs
vec4 pointPosition();

void main()
{
    // retrieve control point texture coordinates
    vec2 edge0 = tcTex[1] - tcTex[0];
    vec2 edge1 = tcTex[3] - tcTex[2];

    // bilinearly interpolate texture coordinate across patch
    vec2 t0 = tcTex[0] + edge0 * gl_TessCoord.x;
    vec2 t1 = tcTex[2] + edge1 * gl_TessCoord.x;
    vec2 texCoord = (t1 - t0) * gl_TessCoord.y + t0;


    vec4 position = pointPosition();

    float waveHeight = amplitude*sin(position.x/wavelength+time*waveSpeed);
    vec3 waveNormal = vec3((amplitude/wavelength)*-cos(position.x/wavelength+time*waveSpeed), 1, 0);

    position.y += waveHeight;

    vPos = position.xyz;
    vNor = waveNormal;
    vTex = texCoord;

    // output patch point position in clip space
    gl_Position = projection*view*position;
}