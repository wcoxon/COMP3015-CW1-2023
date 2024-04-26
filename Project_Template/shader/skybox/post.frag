#version 460

layout (location = 0) in vec3 vPos;

out vec4 colour;

layout(binding = 5) uniform sampler2D renderTexture;



void main()
{
    //blurring
    float weights[3][3] = {{0.25, 0.5, 0.25},{0.5,1,0.5},{0.25,0.5,0.25}};


    float d = 0.005;

    colour = texture(renderTexture,vPos.xy + vec2(-d,-d)) * 0.25;
    colour += texture(renderTexture,vPos.xy + vec2(0,-d)) * 0.5;
    colour += texture(renderTexture,vPos.xy + vec2(d,-d)) * 0.25;

    colour += texture(renderTexture,vPos.xy + vec2(-d,0)) * 0.5;
    colour += texture(renderTexture,vPos.xy + vec2(0,0));
    colour += texture(renderTexture,vPos.xy + vec2(d,0)) * 0.5;

    colour += texture(renderTexture,vPos.xy + vec2(-d,d)) * 0.25;
    colour += texture(renderTexture,vPos.xy + vec2(0,d))*0.5;
    colour += texture(renderTexture,vPos.xy + vec2(d,d)) * 0.25;

    colour /= 4;
}