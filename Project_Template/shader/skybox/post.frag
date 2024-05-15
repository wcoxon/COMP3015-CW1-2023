#version 460

layout (location = 0) in vec3 vPos;
out vec4 colour;

layout(binding = 5) uniform sampler2D renderTexture;
layout(binding = 6) uniform sampler2D depthTexture;

uniform float width=800;
uniform float height=600;

uniform int pass_ID;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

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

void sample_neighbours(sampler2D image, vec2 TexCoords,bool horizontal)
{             
    vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel
    vec3 result = texture(image, TexCoords).rgb * weight[0]; // current fragment's contribution
    if(horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(image, TexCoords + vec2( i/width, 0)).rgb * weight[i];
            result += texture(image, TexCoords - vec2(i/width, 0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(image, TexCoords + vec2(0, i/height)).rgb * weight[i];
            result += texture(image, TexCoords - vec2(0, i/height)).rgb * weight[i];
        }
    }
    colour = vec4(result, 1.0);
}


void main()
{

    switch(pass_ID){
	case 0:
		vec4 n[9];
		make_kernel( n, depthTexture, vPos.xy );

		vec4 sobel_edge_h = n[2] + (2.0*n[5]) + n[8] - (n[0] + (2.0*n[3]) + n[6]);
  		vec4 sobel_edge_v = n[0] + (2.0*n[1]) + n[2] - (n[6] + (2.0*n[7]) + n[8]);
		vec4 sobel = sqrt((sobel_edge_h * sobel_edge_h) + (sobel_edge_v * sobel_edge_v));
		colour = texture(renderTexture,vPos.xy);
		colour *= sobel.r <0.01 ? 1:0 ;
		break;

	case 1:
		//horizontal blur
		sample_neighbours(renderTexture,vPos.xy,true);
		break;

	case 2:
		//vertical blur
		sample_neighbours(renderTexture,vPos.xy,false);
		break;

	default:
		colour = texture(renderTexture,vPos.xy);
		break;
    }
}