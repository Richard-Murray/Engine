#version 410

in vec2 frag_texcoord;
in vec3 frag_normal;
in vec4 geom_worldPosition;

out vec4 out_colour;
uniform sampler2D perlin_texture;

uniform sampler2D grass_texture;
uniform sampler2D rock_texture;
uniform sampler2D snow_texture;

uniform vec3 light_direction;

uniform vec3 camera_position;

void main()
{
	float perlinHeight = texture(perlin_texture, frag_texcoord).r;
	
	out_colour = vec4(1, 0, 0, 1);
	

	vec3 grassColour = texture(grass_texture, frag_texcoord).rgb;
	vec3 snowColour = texture(snow_texture, frag_texcoord).rgb;
	vec3 rockColour = texture(rock_texture, frag_texcoord).rgb;
	
	out_colour.rgb = mix(grassColour, snowColour, perlinHeight / 0.5 - 0.8);
	
	if(perlinHeight < 0.4)
	{
		out_colour = texture(grass_texture, frag_texcoord);
	}
	else if(perlinHeight >= 0.4 && perlinHeight <= 0.5)
	{
		out_colour.rgb = mix(grassColour, rockColour, perlinHeight / 0.1 - 4);
	}
	
	if(perlinHeight >= 0.5 && perlinHeight <= 0.6)
	{
		out_colour = texture(rock_texture, frag_texcoord);
	}
	else if(perlinHeight >= 0.6 && perlinHeight <= 0.9)
	{
		out_colour.rgb = mix(rockColour, snowColour, perlinHeight / 0.3 - (0.6 / 0.3));
	}
	
	if(perlinHeight > 0.9)
	{
		out_colour = texture(snow_texture, frag_texcoord);
	}
		
	//out_colour = texture(grass_texture, frag_texcoord);
	
	//vec3 normColour = frag_normal * 0.5 + 0.5;
	
	//out_colour.rgb = normColour;
	
	float d = max(0, dot( normalize(frag_normal.xyz), light_direction ) ); 
	
	vec3 E = normalize(camera_position - geom_worldPosition.xyz); 
	vec3 R = reflect(-light_direction, frag_normal.xyz); 
	float s = max(0, dot(E, R)); 
	s = pow( s, 1.0f); //specularpower
	
	vec3 light_colour = vec3(1.0f, 1.0f, 1.0f);
	
	out_colour.rgb = out_colour.rgb * d;
	out_colour.a = 1;
	
	//
	out_colour = out_colour * vec4(light_colour * d + light_colour * s, 1);
}