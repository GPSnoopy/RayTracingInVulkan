#version 460
#extension GL_ARB_separate_shader_objects : enable

//layout(binding = 1) uniform sampler2D TextureSampler;

layout(location = 0) in vec3 FragColor;
layout(location = 1) in vec3 FragNormal;
layout(location = 2) in vec2 FragTexCoord;

layout(location = 0) out vec4 OutColor;

void main() 
{
	const vec3 lightVector = normalize(vec3(5, 4, 3));
	const float d = max(dot(lightVector, normalize(FragNormal)), 0.2);

    OutColor = vec4(FragColor * d, 1);// * texture(TextureSampler, FragTexCoord);
}