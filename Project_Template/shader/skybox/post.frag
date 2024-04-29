#version 460

layout (location = 0) in vec3 vPos;

out vec4 colour;

layout(binding = 5) uniform sampler2D renderTexture;
layout(binding = 6) uniform sampler2D depthTexture;

uniform float width=800;
uniform float height=600;


void make_kernel(inout vec4 n[9], sampler2D tex, vec2 coord)
{
	float w = 1.0 / width;
	float h = 1.0 / height;

	n[0] = texture2D(tex, coord + vec2( -w, -h));
	n[1] = texture2D(tex, coord + vec2(0.0, -h));
	n[2] = texture2D(tex, coord + vec2(  w, -h));
	n[3] = texture2D(tex, coord + vec2( -w, 0.0));
	n[4] = texture2D(tex, coord);
	n[5] = texture2D(tex, coord + vec2(  w, 0.0));
	n[6] = texture2D(tex, coord + vec2( -w, h));
	n[7] = texture2D(tex, coord + vec2(0.0, h));
	n[8] = texture2D(tex, coord + vec2(  w, h));
}

void main()
{

    colour = texture(renderTexture,vPos.xy);

    //vec4 colourKernel[9];
    //make_kernel( colourKernel, renderTexture, vPos.xy );
    //
    //for(int x = 0; x < 9; x++){
    //    if(dot(colourKernel[x].rgb, vec3(0.2126, 0.7152, 0.0722))>1){
    //        colour+=colourKernel[x];
    //    }
    //}

    //blurring

    //float d = 0.005;
    //
    //colour += texture(renderTexture,vPos.xy + vec2(-d,-d)) * 0.25;
    //colour += texture(renderTexture,vPos.xy + vec2(0,-d)) * 0.5;
    //colour += texture(renderTexture,vPos.xy + vec2(d,-d)) * 0.25;
    //
    //colour += texture(renderTexture,vPos.xy + vec2(-d,0)) * 0.5;
    //colour += texture(renderTexture,vPos.xy + vec2(d,0)) * 0.5;
    //
    //colour += texture(renderTexture,vPos.xy + vec2(-d,d)) * 0.25;
    //colour += texture(renderTexture,vPos.xy + vec2(0,d))*0.5;
    //colour += texture(renderTexture,vPos.xy + vec2(d,d)) * 0.25;
    //
    //colour /= 4;

    vec4 n[9];
    make_kernel( n, depthTexture, vPos.xy );

    vec4 sobel_edge_h = n[2] + (2.0*n[5]) + n[8] - (n[0] + (2.0*n[3]) + n[6]);
  	vec4 sobel_edge_v = n[0] + (2.0*n[1]) + n[2] - (n[6] + (2.0*n[7]) + n[8]);
	vec4 sobel = sqrt((sobel_edge_h * sobel_edge_h) + (sobel_edge_v * sobel_edge_v));

    

    //colour = texture(renderTexture,vPos.xy);
    colour *= sobel.r <0.01 ? 1:0 ;

    //float brightness = dot(colour.rgb, vec3(0.2126, 0.7152, 0.0722));
    
    //if(brightness<1) colour = vec4(0.0, 0.0, 0.0, 1.0);

}