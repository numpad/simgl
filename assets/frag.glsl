#version 330 core

uniform vec2 viewport;
uniform sampler2D teximage;

in vec2 texcoord;
in vec3 normal;
out vec4 FragColor;

void main() {
	vec2 ScreenPos = gl_FragCoord.xy / viewport;
	vec4 pixel = texture(teximage, texcoord);
	
	FragColor = pixel * (normal.z * -1.0 + 0.4);
}

