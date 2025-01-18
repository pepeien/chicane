#version 450

#extension GL_EXT_nonuniform_qualifier : enable

layout(set = 1, binding = 0) uniform sampler2D material[];

layout(location = 0) out vec4 outColor;

layout(location = 0) in flat int inTextureIndex;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec3 inView;
layout(location = 4) in vec3 inLight;
layout(location = 5) in vec4 inShadow;

#define ambient 0.1

float textureProj(vec4 shadowCoord, vec2 off)
{
	float shadow = 1.0;

	if (shadowCoord.z > -1.0 && shadowCoord.z < 1.0) 
	{
		float dist = texture(material[inTextureIndex], shadowCoord.st + off).r;

		if (shadowCoord.w > 0.0 && dist < shadowCoord.z) 
		{
			shadow = ambient;
		}
	}

	return shadow;
}

float filterPCF(vec4 sc)
{
	ivec2 texDim = textureSize(material[inTextureIndex], 0);
	float scale  = 1.5;
	float dx     = scale * 1.0 / float(texDim.x);
	float dy     = scale * 1.0 / float(texDim.y);

	float shadowFactor = 0.0;
	int count          = 0;
	int range          = 1;

	for (int x = -range; x <= range; x++)
	{
		for (int y = -range; y <= range; y++)
		{
			shadowFactor += textureProj(sc, vec2(dx*x, dy*y));
			count++;
		}
	
	}

	return shadowFactor / count;
}


void main() {
    float shadow = filterPCF(inShadow / inShadow.w);

    vec3 N       = normalize(inNormal);
	vec3 L       = normalize(inLight);
	vec3 V       = normalize(inView);
	vec3 R       = normalize(-reflect(L, N));
	vec3 diffuse = max(dot(N, L), ambient) * inColor;

	outColor = vec4(diffuse * shadow, 1.0);
}