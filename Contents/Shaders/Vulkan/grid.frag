#version 450

layout(location = 0) out vec4 outColor;

layout(location = 1) in vec4 inColor;

void main() 
{
	outColor = inColor.rgba;
}