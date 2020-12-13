#version 330 core
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTextureCoordinates;
layout (location = 3) in vec3 vTangent;
layout (location = 4) in vec3 vBitangent;

out vec3 fPosition;
out vec3 fNormal;
out vec2 fTextureCoordinates;
out vec3 fTangent;
out vec3 fBitangent;
out vec3 fLightDirection;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model_pos;

uniform vec4 LightPosition;

void main()
{
    vec4 Position = view * model_pos * vec4(vPosition, 1.0);
    fPosition = Position.xyz/Position.w;
    fNormal = normalize(transpose(inverse(mat3(model_pos))) * vNormal);
    fLightDirection = normalize(LightPosition.xyz/LightPosition.w - fPosition); 
    fTangent = normalize(transpose(inverse(mat3(model_pos))) * vTangent);
    fBitangent = normalize(transpose(inverse(mat3(model_pos))) * vBitangent);
    fTextureCoordinates = vTextureCoordinates;
    gl_Position = proj * Position;
}