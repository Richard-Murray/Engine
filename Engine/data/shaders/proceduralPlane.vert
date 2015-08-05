#version 410

layout(location=0) in vec4 position;
layout(location=3) in vec2 texcoord;

uniform mat4 view_proj;
uniform sampler2D perlin_texture;

out vec2 geom_texcoord;
out vec4 geom_worldPosition;

void main()
{
	vec4 pos = position;
	pos.y += texture(perlin_texture, texcoord).r * 100;
	
	geom_texcoord = texcoord;
	geom_worldPosition = pos;
}