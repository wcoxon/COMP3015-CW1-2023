#version 460

layout (quads, fractional_odd_spacing, ccw) in;

//uniform sampler2D heightMap;  // the texture corresponding to our height map
uniform mat4 model;           // the model matrix
uniform mat4 view;            // the view matrix
uniform mat4 projection;      // the projection matrix
uniform float time;




//out float depth;

//unit length of wave
uniform float wavelength = 2;
//units per second
uniform float speed = 2;
//vertical scale
uniform float amplitude = 1;

void main()
{
    // get patch coordinate /// this is like a texcoord type of thing for interpolation, think like a lerp factor
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    // ----------------------------------------------------------------------
    // retrieve control point texture coordinates
    //vec2 d00 = tcTex[0];
    //vec2 d01 = tcTex[1];
    //vec2 d10 = tcTex[2];
    //vec2 d11 = tcTex[3];
    // bilinearly interpolate texture coordinate across patch
    //vec2 t0 = (t01 - t00) * u + t00;
    //vec2 t1 = (t11 - t10) * u + t10;
    //vec2 texCoord = (t1 - t0) * v + t0;

    // ----------------------------------------------------------------------
    // retrieve control point pos
    //vec3 pos00 = tcPos[0];
    //vec3 pos01 = tcPos[1];
    //vec3 pos10 = tcPos[2];
    //vec3 pos11 = tcPos[3];
    // bilinearly interpolate pos across patch
    //vec3 pos0 = (pos01-pos00)*u + pos00;
    //vec3 pos1 = (pos11-pos10)*u + pos10;
    //vec3 position = (pos1 - pos0)*v + pos0;


    // lookup texel at patch coordinate for height and scale + shift as desired
    //Height = texture(heightMap, texCoord).y * 64.0 - 16.0;

    // ----------------------------------------------------------------------
    // retrieve control point position coordinates
    vec4 p00 = gl_in[0].gl_Position;
    vec4 p01 = gl_in[1].gl_Position;
    vec4 p10 = gl_in[2].gl_Position;
    vec4 p11 = gl_in[3].gl_Position;

    // compute patch surface normal
    //vec4 uVec = p01 - p00;
    //vec4 vVec = p10 - p00;
    //vec4 normal = normalize( vec4(cross(vVec.xyz, uVec.xyz), 0) );

    // bilinearly interpolate position coordinate across patch
    vec4 p0 = (p01 - p00) * u + p00;
    vec4 p1 = (p11 - p10) * u + p10;
    vec4 p = (p1 - p0) * v + p0;

    // displace point along normal
    //p += normal * Height;

    //wave stuff

    vec4 worldPos = p;

    vec3 waveDisplacement = vec3(0, amplitude*sin(worldPos.x/wavelength+time*speed), 0);
    vec3 waveNormal = vec3((amplitude/wavelength)*-cos(worldPos.x/wavelength+time*speed), 1, 0);

    worldPos += vec4(waveDisplacement,0);

    p = worldPos;
    //vec4 clipPos =  projection*view*model*p;

   //depth = (gl_Position.z+1)/2;

    // ----------------------------------------------------------------------
    // output patch point position in clip space
    gl_Position = p; //model*vec4(position,1);
}