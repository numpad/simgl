#version 330 core

uniform mat4 uView;
uniform mat4 uProjection;

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aInstanceColor;
layout (location = 4) in mat4 aInstanceModel;

out vec3 vPosition;
out vec3 vNormal;
out vec2 vTexCoord;
out vec3 vColor;

void main() {
	vPosition = aInstanceModel * vec4(aPosition, 1.0);
	vNormal = transpose(inverse(mat3(aInstanceModel))) * aNormal;
	vTexCoord = aTexCoord;
	vColor = aInstanceColor;
	gl_Position = uProjection * uView * aInstanceModel * vec4(aPosition, 1.0);
}

