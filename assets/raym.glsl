#version 330 core
#define MAX_STEPS 500
#define MAX_DIST 100.0
#define SURF_DIST 0.005

uniform vec2 uResolution;
uniform float uTime;
uniform vec3 uCamera;

out vec4 FragColor;

float raymarch(vec3 ro, vec3 rd);

/* operations */
float opUnion(float d1, float d2) {
	return min(d1, d2);
}
float opSubstract(float d1, float d2) {
	return max(-d1, d2);
}
float opIntersect(float d1, float d2) {
	return max(d1, d2);
}

float opSmoothUnion( float d1, float d2, float k ) {
	float h = clamp( 0.5 + 0.5*(d2-d1)/k, 0.0, 1.0 );
	return mix( d2, d1, h ) - k*h*(1.0-h);
}

float opSmoothSubstract( float d1, float d2, float k ) {
	float h = clamp( 0.5 - 0.5*(d2+d1)/k, 0.0, 1.0 );
	return mix( d2, -d1, h ) + k*h*(1.0-h);
}

float opSmoothIntersect( float d1, float d2, float k ) {
	float h = clamp( 0.5 - 0.5*(d2-d1)/k, 0.0, 1.0 );
	return mix( d2, d1, h ) + k*h*(1.0-h);
}

/* primitives */
float sdSphere(vec3 p, float r) {
	return length(p) - r;
}

float sdCapsule(vec3 p, vec3 a, vec3 b, float r) {
	vec3 pa = p - a,
	     ba = b - a;
	float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
	return length(pa - ba * h) - r;
}

float sdTorus(vec3 p, vec2 t) {
	vec2 q = vec2(length(p.xz) - t.x, p.y);
	return length(q) - t.y;
}

float sdBox(vec3 p, vec3 b) {
	vec3 d = abs(p) - b;
	return length(max(d,0.0))
		+ min(max(d.x,max(d.y,d.z)),0.0); // remove this line for an only partially signed sdf 
}

float sdRoundBox(vec3 p, vec3 b, float r) {
	vec3 d = abs(p) - b;
	return length(max(d,0.0)) - r
		+ min(max(d.x,max(d.y,d.z)),0.0); // remove this line for an only partially signed sdf 
}

vec3 opRep(vec3 p, vec3 c) {
	return mod(p, c) - 0.5 * c;
}

/* algorithm */
float getdist(vec3 p) {
	float floorDist = p.y;
	float s1 = sdSphere(p - vec3(-1.5 + (sin(uTime * 0.4) * 2), 2, 4), 1);
	float s2 = sdSphere(p - vec3(1.5 - (sin(uTime * 0.4) * 2), 2, 4), 1);
	
	vec3 bp = p - vec3(-3.5, 2, 6);
	const float k = 10.0;
	float c = cos(k * bp.y);
	
	//vec3 qq = vec3(m.pxz, p.bp);
	vec3 qq = bp;
	float b1 = sdTorus(opRep(qq, vec3(6, 13, 6)), (1 + cos(uTime * 0.87) * 0.5));
	b1 += 0.1;
	float d = opSmoothUnion(s1, s2, 2);
	d = opSmoothUnion(d, b1, 1);
	d = opUnion(d, floorDist);
	return d;
}

vec3 getnormal(vec3 p) {
	float d = getdist(p);
	vec2 e = vec2(0.001, 0);

	vec3 n = d - vec3(
		getdist(p - e.xyy),
		getdist(p - e.yxy),
		getdist(p - e.yyx)
	);

	return normalize(n);
}

float getlight(vec3 p) {
	vec3 lightpos = vec3(0, 15.5, -1); //vec3(0, 5, 3);
	lightpos.xz += vec2(sin(uTime), cos(uTime));
	vec3 l = normalize(lightpos - p);
	vec3 n = getnormal(p);
	
	float dif = clamp(dot(n, l), 0.0, 1.0);
	
	/* shadow */
	float d = raymarch(p + n * SURF_DIST * 2, l);
	if (d < length(lightpos - p)) dif *= 0.15;
	
	return dif;
}

float raymarch(vec3 ro, vec3 rd) {
	float total_dist = 0;

	for (int i = 0; i < MAX_STEPS; ++i) {
		vec3 p = ro + rd * total_dist;
		float dS = getdist(p);
		total_dist += dS;
		if (total_dist > MAX_DIST || dS < SURF_DIST) break;
	}

	return total_dist;
}

void main() {
	vec2 uv = (gl_FragCoord.xy - 0.5 * uResolution.xy) / uResolution.y;
	
	vec3 ro = uCamera;
	vec3 rd = normalize(vec3(uv.x, uv.y, 1.0));

	float d = raymarch(ro, rd);
	vec3 p = ro + rd * d;
	float dif = getlight(p);
	
	vec3 color = vec3(dif);
	if (d >= MAX_DIST * 0.9) {
		float p = 1.0 - (d - MAX_DIST * 0.9) / (MAX_DIST * 0.1);
		color *= max(0.1, p);
	}

	FragColor = vec4(color, 1.0);
}

