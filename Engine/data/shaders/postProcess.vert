#version 410

layout(location=0) in vec4 Position;
layout(location=1) in vec2 TexCoord;

out vec2 fTexCoord;

void main()
{
	gl_Position = position;
	fTexCoord = texCoord;
}