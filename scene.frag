#version 330 core
out vec4 color;

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTextureCoordinates;
in vec3 fTangent;
in vec3 fBitangent;

in vec3 fLightDirection;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_height1;
uniform sampler2D texture_ambient1;

uniform vec4 LightDiffuse = vec4(1.0,1.0,1.0,1.0);
uniform vec4 LightAmbient = vec4(0.2,0.2,0.2,1.0);
uniform float Shine = 10.0f;

void main()
{
	vec3 ViewerDirection = normalize(-fPosition);
	vec3 nfLightDirection = normalize(fLightDirection);
	vec3 HalfAngle = normalize(normalize(ViewerDirection) + nfLightDirection);

	vec4 Diffuse = vec4(texture2D(texture_diffuse1, fTextureCoordinates).rgba);
	vec4 Specular = vec4(texture2D(texture_specular1, fTextureCoordinates).rgb, texture2D(texture_diffuse1, fTextureCoordinates).a);
	vec3 Height = vec3(texture2D(texture_height1, fTextureCoordinates).rgb);
	vec4 Ambient = vec4(texture2D(texture_ambient1, fTextureCoordinates).rgb, texture2D(texture_diffuse1, fTextureCoordinates).a);

	//Height = (Height - vec3(0.5, 0.5, 0.5)) * 2.0;
	vec3 RealNormal = vec3(fNormal.x * Height.x, fNormal.y * Height.y, fNormal.z * Height.z);

	vec4 AmbientReflection = LightAmbient * Ambient;

	float DiffuseFactor = clamp(dot(fLightDirection, RealNormal), 0.0, 1.0);
	vec4 DiffuseReflection = LightDiffuse * Diffuse * DiffuseFactor;
	
	float SpecularFactor = pow(clamp(dot(HalfAngle, RealNormal), 0.0, 1.0), Shine);
	vec4 SpecularReflection = LightDiffuse * Specular * SpecularFactor;

	if (dot(fLightDirection, RealNormal) <= 0.0)
	{
		SpecularReflection = vec4(0.0, 0.0, 0.0, 0.0);
	};

	color = AmbientReflection + DiffuseReflection + SpecularReflection;
}