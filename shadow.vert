#version 330 core
layout(location = 0) in vec3 vPosition;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model_pos;

void main()
{
    gl_Position = proj * view * model_pos * vec4(vPosition, 1.0);
}