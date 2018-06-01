#version 330 core

uniform mat4 uView;
uniform sampler2D uDiffuse;
uniform sampler2D uNormal;
uniform sampler2D uSpecular;

in vec2 fTexCoord;
in vec3 fNormal;
in vec3 fPosition;

out vec4 FragColor;

void main() {
	const vec3 light = vec3(-1.0, 1.0, 0.0);
	vec3 cam_pos = uView[3].xyz;

	vec3 diff = texture(uDiffuse, fTexCoord).rgb;
	vec3 norm = texture(uNormal, fTexCoord).rgb;
	float spec = texture(uSpecular, fTexCoord).r;
	
	vec3 to_light = light - fPosition;
	vec3 to_eye = cam_pos - fPosition;

	vec3 mid = (to_light - to_eye) * 0.5;
	vec3 np = to_eye + mid;
	float angle = dot(normalize(np), normalize(to_eye));

	vec3 color = vec3(np);
	
	FragColor = vec4(color, 1.0);
}


