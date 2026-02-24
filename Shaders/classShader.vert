#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;
out vec3 fragPos;
out vec3 normCoord;
out vec2 texCoord; //pass this to frag shader
uniform mat4 projection;
uniform mat4 transform;
uniform mat4 view;
void main() {
	gl_Position = projection * view * transform * vec4(aPos, 1.0);
	texCoord = aTex;
	normCoord = mat3(transpose(inverse(transform))) * aNormal;
	fragPos = vec3(transform * vec4(aPos, 1.0));
}