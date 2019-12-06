#include "Random.glsl"
#include "RayPayload.glsl"

#define PI 3.14159265

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
	const vec4 texColor = m.DiffuseTextureId >= 0 ? texture(TextureSamplers[m.DiffuseTextureId], texCoord) : vec4(1);
	const vec4 colorAndDistance = vec4(m.Diffuse.rgb * texColor.rgb, t);
	const vec4 scatter = vec4(normal + RandomInUnitSphere(seed), isScattered ? 1 : 0);

	return RayPayload(colorAndDistance, scatter, seed);
}

// Metallic
RayPayload ScatterMetallic(const Material m, const vec3 direction, const vec3 normal, const vec2 texCoord, const float t, inout uint seed)
{
	const vec3 reflected = reflect(direction, normal);
	const bool isScattered = dot(reflected, normal) > 0;

	const vec4 texColor = m.DiffuseTextureId >= 0 ? texture(TextureSamplers[m.DiffuseTextureId], texCoord) : vec4(1);
	const vec4 colorAndDistance = isScattered ? vec4(m.Diffuse.rgb * texColor.rgb, t) : vec4(1, 1, 1, -1);
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

	const vec4 texColor = m.DiffuseTextureId >= 0 ? texture(TextureSamplers[m.DiffuseTextureId], texCoord) : vec4(1);
	
	return RandomFloat(seed) < reflectProb
		? RayPayload(vec4(texColor.rgb, t), vec4(reflect(direction, normal), 1), seed)
		: RayPayload(vec4(texColor.rgb, t), vec4(refracted, 1), seed);
}

// Specular
RayPayload ScatterSpecular(const Material m, const vec3 direction, const vec3 normal, const vec2 texCoord, const float t, inout uint seed)
{
	const bool isScattered = dot(direction, normal) < 0;
	const vec4 texColor = m.DiffuseTextureId >= 0 ? texture(TextureSamplers[m.DiffuseTextureId], texCoord) : vec4(1);

//	float NdotD = max(0, dot(normal, -direction));
//	float DdotD = max(0, dot(-direction, -direction));
//	float Rs = 0.0;
//	if (NdotD > 0) 
//	{
//		// Fresnel reflectance
//		float F = pow(1.0 - DdotD, 5.0);
//		F *= (1.0 - 0.8);
//		F += 0.8;
//
//		// Microfacet distribution by Beckmann
//		float m_squared = m.Fuzziness * m.Fuzziness;
//		float r1 = 1.0 / (4.0 * m_squared * pow(NdotD, 4.0));
//		float r2 = (NdotD * NdotD - 1.0) / (m_squared * NdotD * NdotD);
//		float D = r1 * exp(r2);
//
//		// Geometric shadowing
//		float two_NdotD = 2.0 * NdotD;
//		float g1 = (two_NdotD * NdotD) / DdotD;
//		float g2 = (two_NdotD * NdotD) / DdotD;
//		float G = min(1.0, min(g1, g2));
//
//		Rs = (F * D * G) / (PI * NdotD * NdotD);
//	}
	
	float F0 = 0.8;
	float k = 0.2;
	vec3 lightDir = -direction;
	vec3 viewDir = -direction;
	float roughness = m.Fuzziness;

	float NdotL = max(0, dot(normal, lightDir));
	float Rs = 0.0;
	if (NdotL > 0) 
	{
		vec3 H = normalize(lightDir + viewDir);
		float NdotH = max(0, dot(normal, H));
		float NdotV = max(0, dot(normal, viewDir));
		float VdotH = max(0, dot(lightDir, H));

		// Fresnel reflectance
		float F = pow(1.0 - VdotH, 5.0);
		F *= (1.0 - F0);
		F += F0;

		// Microfacet distribution by Beckmann
		float m_squared = roughness * roughness;
		float r1 = 1.0 / (4.0 * m_squared * pow(NdotH, 4.0));
		float r2 = (NdotH * NdotH - 1.0) / (m_squared * NdotH * NdotH);
		float D = r1 * exp(r2);

		// Geometric shadowing
		float two_NdotH = 2.0 * NdotH;
		float g1 = (two_NdotH * NdotV) / VdotH;
		float g2 = (two_NdotH * NdotL) / VdotH;
		float G = min(1.0, min(g1, g2));

		Rs = (F * D * G) / (PI * NdotL * NdotV);
	}

	vec3 col = m.Diffuse.rgb * texColor.rgb * NdotL + NdotL * (k + Rs * (1.0 - k));

	const vec4 colorAndDistance = vec4(col, t);
	const vec4 scatter = vec4(normal + RandomInUnitSphere(seed), isScattered ? 1 : 0);

	return RayPayload(colorAndDistance, scatter, seed);
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
	case MaterialSpecular:
		return ScatterSpecular(m, normDirection, normal, texCoord, t, seed);
	case MaterialDiffuseLight:
		return ScatterDiffuseLight(m, t, seed);
	}
}

