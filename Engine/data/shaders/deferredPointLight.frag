#version 410

out vec3 LightOutput;

uniform vec3 lightPositionView;
uniform vec3 lightDiffuse;
uniform float lightRadius;
//uniform vec3 cameraPos
//uniform float specularPower;

uniform sampler2D positionTexture;
uniform sampler2D normalTexture;

void main() {
// calculate the texture coordinates for the fragment
	vec2 texCoord = gl_FragCoord.xy / textureSize(positionTexture, 0).xy;
	
	vec3 normal = normalize(texture(normalTexture, texCoord).xyz);
	vec3 position = texture(positionTexture, texCoord).xyz;
	
	vec3 toLight = lightPositionView - position;
	
	float d = max(0, dot(normal, normalize(toLight)));
	
	// linear falloff
	float falloff = 1 - min(1, length(toLight) / lightRadius);
	
	LightOutput = lightDiffuse * d * falloff;//lightDiffuse * d * falloff;
	
	//Specular
	//vec3 E = normalize(cameraPos - position);
	//vec3 R = reflect(-toLight, normal);
	//float s = max(0, dot(R, E));
	//s = pow(s, SpecPow);
	//specularOutput = LightColour * s * falloff * 0.1f;
}