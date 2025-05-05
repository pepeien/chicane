#version 450

#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 3) uniform sampler2D shadowMap;

layout(set = 1, binding = 0) uniform sampler2D textures[];

layout(location = 0) in flat int inTextureIndex;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec4 inColor;
layout(location = 4) in vec3 inViewPosition;
layout(location = 5) in vec3 inLightPosition;
layout(location = 8) in vec4 inShadowPosition; 

#define ambient 0.1

float textureProj(vec4 shadowCoord, vec2 off)
{
	float shadow = 1.0;

	if (shadowCoord.z > 0.0 && shadowCoord.z < 1.0) 
	{
		float dist = texture(shadowMap, shadowCoord.st + off).r;

		if (dist < shadowCoord.z) 
		{
			shadow = ambient;
		}
	}

	return shadow;
}

float filterPCF(vec4 sc)
{
	ivec2 texDim = textureSize(shadowMap, 0);

	float dx = 1.0 / float(texDim.x);
	float dy = 1.0 / float(texDim.y);

	float shadowFactor = 0.0;

	int count = 0;
	int range = 1;
	
	for (int x = -range; x <= range; x++)
	{
		for (int y = -range; y <= range; y++)
		{
			shadowFactor += textureProj(sc, vec2(dx*x, dy*y));

			count++;
		}
	}

	return shadowFactor / float(count);
}

void main() 
{	
	float shadow = filterPCF(inShadowPosition / inShadowPosition.w);

	vec3 N = normalize(inNormal);
	vec3 L = normalize(inLightPosition);
	vec3 V = normalize(inViewPosition);
	vec3 R = normalize(-reflect(L, N));

    vec4 textureColor = texture(textures[inTextureIndex], inUV);

	vec3 diffuse = max(dot(N, L), 0.0) * textureColor.rgb;

	outColor = vec4(diffuse * shadow, textureColor.a);
}