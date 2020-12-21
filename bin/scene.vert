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
out vec4 fPosLightSpace;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model_pos;

uniform vec3 light_pos;
uniform mat4 light_view;
uniform mat4 light_proj;
uniform mat4 normal_matrix;
out vec2 TexCoords;

void main()
{
    TexCoords = vec2(vPosition.x, vPosition.y);
    vec4 Position = view * model_pos * vec4(vPosition, 1.0);
    fPosition = Position.xyz/Position.w;

    fNormal = normalize((normal_matrix * vec4(vNormal, 1.0)).xyz);
    fLightDirection = normalize(light_pos - fPosition);// направление в сторону света

    fPosLightSpace = light_proj * light_view * model_pos * vec4(vPosition, 1.0);

    fTangent = normalize((normal_matrix * vec4(vTangent, 1.0)).xyz);
    fBitangent = normalize((normal_matrix * vec4(vBitangent, 1.0)).xyz);
    fTextureCoordinates = vTextureCoordinates;
    gl_Position = proj * Position;
}