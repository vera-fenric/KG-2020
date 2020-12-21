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
uniform sampler2D texture_ambient1;
uniform sampler2D shadowMap;

uniform vec3 LightSpecular = vec3(1.0,1.0,1.0);
uniform vec3 LightColor = vec3(1.0,0.8,0.5);
uniform vec3 LightAmbient = vec3(0.2,0.2,0.2);
uniform float Shine = 1.0f;

in vec2 TexCoords;

float ShadowCalculation(vec4 fPosLightSpace)
{
	vec3 fPosLightSpace3 = fPosLightSpace.xyz/fPosLightSpace.w;
	fPosLightSpace3 = fPosLightSpace3 * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, fPosLightSpace3.xy).r;
	float currentDepth = fPosLightSpace3.z;
	float shadow = 0.0;
	float shift;
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

	vec3 ViewerDirection = normalize(-fPosition);
	vec3 HalfAngle = normalize(ViewerDirection + fLightDirection);

	float shadow = ShadowCalculation(fPosLightSpace);

	vec4 TrueColor = vec4(texture2D(texture_diffuse1, fTextureCoordinates));
	//vec4 Specular = vec4(texture2D(texture_specular1, fTextureCoordinates).rgb, texture2D(texture_diffuse1, fTextureCoordinates).a);
	vec3 Normal = (vec3(texture2D(texture_height1, fTextureCoordinates).rgb)- vec3(0.5, 0.5, 0.5)) * 2.0;
	vec3 Ambient = vec3(texture2D(texture_ambient1, fTextureCoordinates).rgb)*TrueColor.rgb*LightAmbient*LightColor;

	vec3 LightDirFromFace = vec3(dot(fLightDirection, fTangent), dot(fLightDirection, fBitangent), dot(fLightDirection, fNormal));

	float DiffuseFactor = clamp(dot(LightDirFromFace, Normal), 0.0, 1.0);
	vec3 Diffuse = DiffuseFactor * TrueColor.rgb * LightColor * (1.0 - shadow);
	
	vec3 HalfAngleFromFace = vec3(dot(HalfAngle, fTangent), dot(HalfAngle, fBitangent), dot(HalfAngle, fNormal));
	float SpecularFactor = pow(clamp(dot(HalfAngleFromFace, Normal), 0.0, 1.0), 30.0);
	vec3 Specular = SpecularFactor * TrueColor.rgb * LightSpecular * (1.0 - shadow);

	if (dot(LightDirFromFace, Normal) <= 0.0)
	{
		Specular = vec3(0.0, 0.0, 0.0);
	};

	color = vec4(Ambient + Diffuse + Specular, TrueColor.a);
}