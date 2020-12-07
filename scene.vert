#version 330 core

in vec3 model_pos;
uniform mat4 mvp;

void main() {
	gl_Position = mvp * vec4(model_pos, 1.0);
}