#version 410

layout(location = 0) in vec4 Position;
layout(location = 1) in vec4 Normal;
layout(location = 2) in vec2 TexCoord;

// view-space normal and position
out vec4 vPosition;
out vec4 vNormal;
out vec2 vTexCoord;

uniform mat4 View; // needed in addition to Projection * View
uniform mat4 ProjectionView;
uniform mat4 WorldTransform;

void main() 
{
	// first store view-space position and normal
	vPosition = View * WorldTransform * Position;
	//vNormal = Normal;
	vNormal = normalize(View * WorldTransform * Normal);
	vTexCoord = TexCoord;
	
	gl_Position = ProjectionView * WorldTransform * Position;
}