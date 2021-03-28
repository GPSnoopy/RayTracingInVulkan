#extension GL_EXT_nonuniform_qualifier : require

#include "Random.glsl"
#include "RayPayload.glsl"

// Polynomial approximation by Christophe Schlick
float Schlick(const float cosine, const float refractionIndex)
{
	float r0 = (1 - refractionIndex) / (1 + refractionIndex);
	r0 *= r0;
	return r0 + (1 - r0) * pow(1 - cosine, 5);
}

// Lambertian
RayPayload ScatterLambertian(const Material m, const vec3 direction, const vec3 normal, const vec2 texCoord, const float t, inout uint seed)
{
	const bool isScattered = dot(direction, normal) < 0;
	const vec4 texColor = m.DiffuseTextureId >= 0 ? texture(TextureSamplers[nonuniformEXT(m.DiffuseTextureId)], texCoord) : vec4(1);
	const vec4 colorAndDistance = vec4(m.Diffuse.rgb * texColor.rgb, t);
	const vec4 scatter = vec4(normal + RandomInUnitSphere(seed), isScattered ? 1 : 0);

	return RayPayload(colorAndDistance, scatter, seed);
}

// Metallic
RayPayload ScatterMetallic(const Material m, const vec3 direction, const vec3 normal, const vec2 texCoord, const float t, inout uint seed)
{
	const vec3 reflected = reflect(direction, normal);
	const bool isScattered = dot(reflected, normal) > 0;

	const vec4 texColor = m.DiffuseTextureId >= 0 ? texture(TextureSamplers[nonuniformEXT(m.DiffuseTextureId)], texCoord) : vec4(1);
	const vec4 colorAndDistance = vec4(m.Diffuse.rgb * texColor.rgb, t);
	const vec4 scatter = vec4(reflected + m.Fuzziness*RandomInUnitSphere(seed), isScattered ? 1 : 0);

	return RayPayload(colorAndDistance, scatter, seed);
}

// Dielectric
RayPayload ScatterDieletric(const Material m, const vec3 direction, const vec3 normal, const vec2 texCoord, const float t, inout uint seed)
{
	const float dot = dot(direction, normal);
	const vec3 outwardNormal = dot > 0 ? -normal : normal;
	const float niOverNt = dot > 0 ? m.RefractionIndex : 1 / m.RefractionIndex;
	const float cosine = dot > 0 ? m.RefractionIndex * dot : -dot;

	const vec3 refracted = refract(direction, outwardNormal, niOverNt);
	const float reflectProb = refracted != vec3(0) ? Schlick(cosine, m.RefractionIndex) : 1;

	const vec4 texColor = m.DiffuseTextureId >= 0 ? texture(TextureSamplers[nonuniformEXT(m.DiffuseTextureId)], texCoord) : vec4(1);
	
	return RandomFloat(seed) < reflectProb
		? RayPayload(vec4(texColor.rgb, t), vec4(reflect(direction, normal), 1), seed)
		: RayPayload(vec4(texColor.rgb, t), vec4(refracted, 1), seed);
}

// Diffuse Light
RayPayload ScatterDiffuseLight(const Material m, const float t, inout uint seed)
{
	const vec4 colorAndDistance = vec4(m.Diffuse.rgb, t);
	const vec4 scatter = vec4(1, 0, 0, 0);

	return RayPayload(colorAndDistance, scatter, seed);
}

RayPayload Scatter(const Material m, const vec3 direction, const vec3 normal, const vec2 texCoord, const float t, inout uint seed)
{
	const vec3 normDirection = normalize(direction);

	switch (m.MaterialModel)
	{
	case MaterialLambertian:
		return ScatterLambertian(m, normDirection, normal, texCoord, t, seed);
	case MaterialMetallic:
		return ScatterMetallic(m, normDirection, normal, texCoord, t, seed);
	case MaterialDielectric:
		return ScatterDieletric(m, normDirection, normal, texCoord, t, seed);
	case MaterialDiffuseLight:
		return ScatterDiffuseLight(m, t, seed);
	}
}

