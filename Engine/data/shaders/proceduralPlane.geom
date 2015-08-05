#version 410

// input is traiangle
// output is a triangle
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 view_proj;

// input data from vertex shader
in vec4 geom_worldPosition[];
in vec2 geom_texcoord[];

out vec2 frag_texcoord;
out vec3 frag_normal;

void main()
{
	vec3 dirOne = normalize((geom_worldPosition[1] - geom_worldPosition[0]).xyz);
	vec3 dirTwo = normalize((geom_worldPosition[2] - geom_worldPosition[0]).xyz);
	frag_normal = cross(dirTwo, dirOne);

	frag_texcoord = geom_texcoord[0];
	gl_Position = view_proj * geom_worldPosition[0];
	EmitVertex();
	
	frag_texcoord = geom_texcoord[1];
	gl_Position = view_proj * geom_worldPosition[1];
	EmitVertex();
	
	frag_texcoord = geom_texcoord[2];
	gl_Position = view_proj * geom_worldPosition[2];
	EmitVertex();
}