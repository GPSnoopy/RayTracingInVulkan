#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_GOOGLE_include_directive : require
#include "Material.glsl"
#include "CookTorrance.glsl"

layout(binding = 1) readonly buffer MaterialArray { Material[] Materials; };
layout(binding = 2) uniform sampler2D[] TextureSamplers;

layout (location = 0) in vec3 inColor;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in flat int inMaterialIndex;
layout (location = 4) in vec3 inLight;

layout(location = 0) out vec4 OutColor;

void main() 
{
	vec3 view = inLight;
	vec3 normal = normalize(inNormal);

	const int textureId = Materials[inMaterialIndex].DiffuseTextureId;
	
	vec3 diffuse = textureId >= 0 ? texture(TextureSamplers[textureId], inUV).rgb : inColor;

	vec3 ct = CookTorrance(diffuse, normal, inLight, view, 0.5);

    OutColor = vec4(ct, 1);
}