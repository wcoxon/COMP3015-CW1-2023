#version 460

// uniforms
uniform float time;
uniform float wavelength;
uniform float waveSpeed;
uniform float amplitude;

// common.tese stubs
vec4 pointPosition();

void main()
{
    vec4 position = pointPosition();

    //wave stuff
    float waveHeight = amplitude*sin(position.x/wavelength+time*waveSpeed);

    position.y += waveHeight;



    // output patch point position in clip space
    gl_Position = position;
}