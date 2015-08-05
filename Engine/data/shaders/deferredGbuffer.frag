#version 410

in vec4 vPosition;
in vec4 vNormal;
in vec2 vTexCoord;

layout(location = 0) out vec3 gpassAlbedo;
layout(location = 1) out vec3 gpassPosition;
layout(location = 2) out vec3 gpassNormal;

uniform sampler2D Diffuse;

void main() 
{
	// we simply output the data
	
	// Note: you could use a material colour,
	// or sample a texture for albedo
	gpassAlbedo = texture(Diffuse, vTexCoord).xyz;

	gpassPosition = vPosition.xyz;
	gpassNormal = vNormal.xyz;
}