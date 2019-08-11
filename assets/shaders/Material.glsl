#include "Random.glsl"
#include "RayPayload.glsl"

const uint MaterialLambertian = 0;
const uint MaterialMetallic = 1;
const uint MaterialDielectric = 2;
const uint MaterialIsotropic = 3;
const uint MaterialDiffuseLight = 4;

struct Material
{
	vec4 Diffuse;
	float Fuzziness;
	float RefractionIndex;
	uint MaterialModel;
};

// Polynomial approximation by Christophe Schlick
float Schlick(const float cosine, const float refractionIndex)
{
	float r0 = (1 - refractionIndex) / (1 + refractionIndex);
	r0 *= r0;
	return r0 + (1 - r0) * pow(1 - cosine, 5);
}

// Lambertian
RayPayload ScatterLambertian(const Material m, const vec3 direction, const vec3 normal, const float t, inout uint seed)
{
	const bool isScattered = dot(direction, normal) < 0;
	const vec4 colorAndDistance = vec4(m.Diffuse.rgb, t);
	const vec4 scatter = vec4(normal + RandomInUnitSphere(seed), isScattered ? 1 : 0);

	return RayPayload(colorAndDistance, scatter, seed);
}

// Metallic
RayPayload ScatterMetallic(const Material m, const vec3 direction, const vec3 normal, const float t, inout uint seed)
{
	const vec3 reflected = reflect(direction, normal);
	const bool isScattered = dot(reflected, normal) > 0;

	const vec4 colorAndDistance = isScattered ? vec4(m.Diffuse.rgb, t) : vec4(1, 1, 1, -1);
	const vec4 scatter = vec4(reflected + m.Fuzziness*RandomInUnitSphere(seed), isScattered ? 1 : 0);

	return RayPayload(colorAndDistance, scatter, seed);
}

// Dielectric
RayPayload ScatterDieletric(const Material m, const vec3 direction, const vec3 normal, const float t, inout uint seed)
{
	const float dot = dot(direction, normal);
	const vec3 outwardNormal = dot > 0 ? -normal : normal;
	const float niOverNt = dot > 0 ? m.RefractionIndex : 1 / m.RefractionIndex;
	const float cosine = dot > 0 ? m.RefractionIndex * dot : -dot;

	const vec3 refracted = refract(direction, outwardNormal, niOverNt);
	const float reflectProb = refracted != vec3(0) ? Schlick(cosine, m.RefractionIndex) : 1;

	return RandomFloat(seed) < reflectProb
		? RayPayload(vec4(1, 1, 1, t), vec4(reflect(direction, normal), 1), seed)
		: RayPayload(vec4(1, 1, 1, t), vec4(refracted, 1), seed);
}

// Diffuse Light
RayPayload ScatterDiffuseLight(const Material m, const float t, inout uint seed)
{
	const vec4 colorAndDistance = vec4(m.Diffuse.rgb, t);
	const vec4 scatter = vec4(1, 0, 0, 0);

	return RayPayload(colorAndDistance, scatter, seed);
}

RayPayload Scatter(const Material m, const vec3 direction, const vec3 normal, const float t, inout uint seed)
{
	const vec3 normDirection = normalize(direction);

	switch (m.MaterialModel)
	{
	case MaterialLambertian:
		return ScatterLambertian(m, normDirection, normal, t, seed);
	case MaterialMetallic:
		return ScatterMetallic(m, normDirection, normal, t, seed);
	case MaterialDielectric:
		return ScatterDieletric(m, normDirection, normal, t, seed);
	case MaterialDiffuseLight:
		return ScatterDiffuseLight(m, t, seed);
	}
}

