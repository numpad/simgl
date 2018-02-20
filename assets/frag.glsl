#version 330 core

uniform vec2 viewport;
uniform sampler2D teximage;
uniform mat4 projection;
uniform int render_depthbuffer;

in vec2 texcoord;
in vec3 normal;
in vec3 FragPos;
out vec4 FragColor;

void main() {
	vec2 ScreenPos = gl_FragCoord.xy / viewport;
	vec4 pixel = texture(teximage, texcoord);
	
	/* depth */
	float zNear = 0.1;
	float zFar = 350.0;
	float depth = (gl_FragCoord.z * 2.0) - 1.0;
	float z = (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
	
	if (render_depthbuffer == 1) {
		float z0 = 0.0,
			  z1 = 0.0,
			  z2 = 0.0;
		
		if (z < 0.33)
			z0 = z;
		else if (z < 0.66)
			z1 = z;
		else
			z2 = z;
		FragColor = vec4(vec3(z0, z1, z2), 1.0);
	} else {
		FragColor = (1.0 - z * 1.2) * pixel * (normal.y + 0.5);
	}
}

