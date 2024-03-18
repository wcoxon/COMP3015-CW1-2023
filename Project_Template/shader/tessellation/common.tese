#version 460

layout (quads, equal_spacing, ccw) in;

vec4 pointPosition()
{
    vec4 edge0 = gl_in[1].gl_Position - gl_in[0].gl_Position;
    vec4 edge1 = gl_in[3].gl_Position - gl_in[2].gl_Position;

    // bilinearly interpolate position across patch
    vec4 position0 = gl_in[0].gl_Position + edge0 * gl_TessCoord.x;
    vec4 position1 = gl_in[2].gl_Position + edge1 * gl_TessCoord.x;
    vec4 position = position0 + (position1 - position0) * gl_TessCoord.y;

    return position;
}