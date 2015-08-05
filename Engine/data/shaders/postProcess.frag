#version 410

in vec2 fTexCoord;

out vec4 FragColour;

uniform sampler2D target;

vec4 Simple()
{
	return texture(target, fTexCoord);
}

void main()
{
	FragColour = Simple();
}