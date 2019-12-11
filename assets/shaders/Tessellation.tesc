#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_GOOGLE_include_directive : require
#include "UniformBufferObject.glsl"

layout(binding = 0) readonly uniform UniformBufferObjectStruct { UniformBufferObject ubo; };

layout (vertices = 3) out;
 
layout (location = 0) in vec3 inColor[];
layout (location = 1) in vec3 inNormal[];
layout (location = 2) in vec2 inUV[];
layout (location = 3) in int inMaterialIndex[];

layout (location = 0) out vec3 outColor[3];
layout (location = 1) out vec3 outNormal[3];
layout (location = 2) out vec2 outUV[3];
layout (location = 3) out int outMaterialIndex[3];

in gl_PerVertex
{
  vec4 gl_Position;
} gl_in[gl_MaxPatchVertices];
 
void main()
{
	if (gl_InvocationID == 0)
	{
		gl_TessLevelInner[0] = ubo.TessLevel;
		gl_TessLevelOuter[0] = ubo.TessLevel;
		gl_TessLevelOuter[1] = ubo.TessLevel;
		gl_TessLevelOuter[2] = ubo.TessLevel;		
	}

	gl_out[gl_InvocationID].gl_Position =  gl_in[gl_InvocationID].gl_Position;
	outColor[gl_InvocationID] = inColor[gl_InvocationID];
	outNormal[gl_InvocationID] = inNormal[gl_InvocationID];
	outUV[gl_InvocationID] = inUV[gl_InvocationID];
	outMaterialIndex[gl_InvocationID] = inMaterialIndex[gl_InvocationID];
} 