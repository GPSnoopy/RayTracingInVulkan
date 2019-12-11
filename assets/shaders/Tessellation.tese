#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_GOOGLE_include_directive : require
#include "Material.glsl"
#include "UniformBufferObject.glsl"

layout(binding = 0) readonly uniform UniformBufferObjectStruct { UniformBufferObject ubo; };
layout(binding = 1) readonly buffer MaterialArray { Material[] Materials; };
layout(binding = 2) uniform sampler2D[] TextureSamplers;

layout(triangles, equal_spacing, ccw) in;

layout (location = 0) in vec3 inColor[];
layout (location = 1) in vec3 inNormal[];
layout (location = 2) in vec2 inUV[];
layout (location = 3) in int inMaterialIndex[];
 
layout (location = 0) out vec3 outColor;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec2 outUV;
layout (location = 3) out int outMaterialIndex;

int computeMat(vec3 tc) {
    float maximum = max(tc.x, tc.y);
    maximum = max(maximum, tc.z);

	if (maximum == tc.x) return inMaterialIndex[0];
	if (maximum == tc.y) return inMaterialIndex[1];
	return inMaterialIndex[2];
}

void main()
{
	gl_Position = (gl_TessCoord.x * gl_in[0].gl_Position) + (gl_TessCoord.y * gl_in[1].gl_Position) + (gl_TessCoord.z * gl_in[2].gl_Position); 
	outColor = gl_TessCoord.x * inColor[0] + gl_TessCoord.y * inColor[1] + gl_TessCoord.z * inColor[2];
	outUV = gl_TessCoord.x * inUV[0] + gl_TessCoord.y * inUV[1] + gl_TessCoord.z * inUV[2];
	outNormal = gl_TessCoord.x * inNormal[0] + gl_TessCoord.y * inNormal[1] + gl_TessCoord.z * inNormal[2]; 
	outMaterialIndex = computeMat(gl_TessCoord.xyz);
				
	Material m = Materials[outMaterialIndex];
	uint texId = (Materials.length() / 2) + m.DiffuseTextureId;
	gl_Position.xyz += normalize(outNormal) * (max(textureLod(TextureSamplers[texId], outUV.st, 0.0).a, 0.0) * ubo.TessStrength);
		
	gl_Position = ubo.Projection * ubo.ModelView * gl_Position;
}