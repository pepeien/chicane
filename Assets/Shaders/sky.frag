#version 450

layout(set = 1, binding = 0) uniform samplerCube material;

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 inForward;

void main() {
	outColor = texture(material, inForward);
}