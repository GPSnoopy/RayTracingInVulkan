
struct RayPayload
{
	vec4 ColorAndDistance; // xyz + t
	vec4 ScatterDirection; // xyz + w (is scatter needed)
	uint RandomSeed;
};
