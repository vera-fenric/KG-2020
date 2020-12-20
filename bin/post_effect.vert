#version 330 core
layout (location = 0) in vec3 vPosition;

out vec2 TexCoords;

void main()
{
    vec3 Pos = vPosition * 2 - 1.0;
    gl_Position = vec4(Pos.x, Pos.y, 0.0, 1.0); 
    TexCoords = vec2(vPosition.x, vPosition.y);
}  