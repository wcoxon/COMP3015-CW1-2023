#version 460

layout (quads, fractional_odd_spacing, ccw) in;

// uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float time;

uniform float wavelength; // wave length (units)
uniform float waveSpeed; // waves speed (units per second)
uniform float amplitude; // waves height (units)

void main()
{
    // get patch coordinate
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    // retrieve control point position coordinates
    vec4 position00 = gl_in[0].gl_Position;
    vec4 position01 = gl_in[1].gl_Position;
    vec4 position10 = gl_in[2].gl_Position;
    vec4 position11 = gl_in[3].gl_Position;

    // bilinearly interpolate position coordinate across patch
    vec4 position0 = (position01 - position00) * u + position00;
    vec4 position1 = (position11 - position10) * u + position10;
    vec4 position = (position1 - position0) * v + position0;

    //wave stuff
    //vec4 worldPos = position;

    vec3 waveDisplacement = vec3(0, amplitude*sin(position.x/wavelength+time*waveSpeed), 0);
    vec3 waveNormal = vec3((amplitude/wavelength)*-cos(position.x/wavelength+time*waveSpeed), 1, 0);

    position += vec4(waveDisplacement,0);

    //position = worldPos;

    // output patch point position in clip space
    gl_Position = position;
}