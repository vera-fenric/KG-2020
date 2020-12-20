#version 330 core
out vec4 color;

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTextureCoordinates;
in vec3 fTangent;
in vec3 fBitangent;

in vec3 fLightDirection;
in vec4 fPosLightSpace;

uniform sampler2D texture_diffuse1;
//uniform sampler2D texture_specular1;
uniform sampler2D texture_height1;
//uniform sampler2D texture_ambient1;
uniform sampler2D shadowMap;

uniform vec4 LightDiffuse = vec4(1.0,1.0,1.0,1.0);
uniform vec4 LightAmbient = vec4(0.4,0.4,0.4,1.0);
uniform float Shine = 10.0f;

in vec2 TexCoords;

float ShadowCalculation(vec4 fPosLightSpace)
{
	vec3 fPosLightSpace3 = fPosLightSpace.xyz/fPosLightSpace.w;
	fPosLightSpace3 = fPosLightSpace3 * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, fPosLightSpace3.xy).r;
	float currentDepth = fPosLightSpace3.z;
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -2; x <= 2; ++x)		//усредняем значение тени
	{
		for(int y = -2; y <= 2; ++y)
		{
			float closestDepth = texture(shadowMap, fPosLightSpace3.xy + vec2(x, y) * texelSize).r;
			if ((currentDepth - 0.0005)> closestDepth)
					shadow = shadow + 1.0;
		}
	}
	shadow = shadow/25.0;
	return shadow;
}



void main()
{
	//color = texture(shadowMap, TexCoords);
	//float shadow = ShadowCalculation(fPosLightSpace);
	//color = vec4((1.0-shadow)*vec3(1.0, 1.0, 1.0), 1.0);
	//color = vec4((1-ShadowCalculation(fPosLightSpace)) * vec3(0.5,0.5,0.5),1.0);
	///*
	vec3 ViewerDirection = normalize(-fPosition);
	vec3 HalfAngle = normalize(normalize(ViewerDirection) + fLightDirection);

	float shadow = ShadowCalculation(fPosLightSpace);

	vec4 Diffuse = vec4(texture2D(texture_diffuse1, fTextureCoordinates).rgba);
	//vec4 Specular = vec4(texture2D(texture_specular1, fTextureCoordinates).rgb, texture2D(texture_diffuse1, fTextureCoordinates).a);
	vec3 Height = vec3(texture2D(texture_height1, fTextureCoordinates).rgb);
	//vec4 Ambient = vec4(texture2D(texture_ambient1, fTextureCoordinates).rgb, texture2D(texture_diffuse1, fTextureCoordinates).a);

	//Height = (Height - vec3(0.5, 0.5, 0.5)) * 2.0;
	vec3 RealNormal = vec3(fNormal.x * Height.x, fNormal.y * Height.y, fNormal.z * Height.z);

	vec4 AmbientReflection = LightAmbient * Diffuse;

	float DiffuseFactor = clamp(dot(fLightDirection, RealNormal), 0.0, 1.0);
	vec4 DiffuseReflection = LightDiffuse * Diffuse * DiffuseFactor * (1.0 - shadow);
	
	float SpecularFactor = pow(clamp(dot(HalfAngle, RealNormal), 0.0, 1.0), Shine);
	vec4 SpecularReflection = LightDiffuse * Diffuse * SpecularFactor * (1.0 - shadow);

	if (dot(fLightDirection, RealNormal) <= 0.0)
	{
		SpecularReflection = vec4(0.0, 0.0, 0.0, 0.0);
	};

	color = AmbientReflection + DiffuseReflection + SpecularReflection;
	//*/
}