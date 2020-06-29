
// Profiling DXR Shaders with Timer Instrumentation
// https://developer.nvidia.com/blog/profiling-dxr-shaders-with-timer-instrumentation/
vec3 heatmap(float t)
{
	const vec3 c[10] = {
		vec3(0.0f / 255.0f,   2.0f / 255.0f,  91.0f / 255.0f),
		vec3(0.0f / 255.0f, 108.0f / 255.0f, 251.0f / 255.0f),
		vec3(0.0f / 255.0f, 221.0f / 255.0f, 221.0f / 255.0f),
		vec3(51.0f / 255.0f, 221.0f / 255.0f,   0.0f / 255.0f),
		vec3(255.0f / 255.0f, 252.0f / 255.0f,   0.0f / 255.0f),
		vec3(255.0f / 255.0f, 180.0f / 255.0f,   0.0f / 255.0f),
		vec3(255.0f / 255.0f, 104.0f / 255.0f,   0.0f / 255.0f),
		vec3(226.0f / 255.0f,  22.0f / 255.0f,   0.0f / 255.0f),
		vec3(191.0f / 255.0f,   0.0f / 255.0f,  83.0f / 255.0f),
		vec3(145.0f / 255.0f,   0.0f / 255.0f,  65.0f / 255.0f)
	};

	const float s = t * 10.0f;

	const int cur = int(s) <= 9 ? int(s) : 9;
	const int prv = cur >= 1 ? cur - 1 : 0;
	const int nxt = cur < 9 ? cur + 1 : 9;

	const float blur = 0.8f;

	const float wc = smoothstep(float(cur) - blur, float(cur) + blur, s) * (1.0f - smoothstep(float(cur + 1) - blur, float(cur + 1) + blur, s));
	const float wp = 1.0f - smoothstep(float(cur) - blur, float(cur) + blur, s);
	const float wn = smoothstep(float(cur + 1) - blur, float(cur + 1) + blur, s);

	const vec3 r = wc * c[cur] + wp * c[prv] + wn * c[nxt];
	return vec3(clamp(r.x, 0.0f, 1.0f), clamp(r.y, 0.0f, 1.0f), clamp(r.z, 0.0f, 1.0f));
}