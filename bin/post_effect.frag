#version 330 core
out vec4 color;
  
in vec2 TexCoords;

uniform int Effect;
uniform sampler2D screenTexture;

vec3 MyEffect(vec4 OldColor)
{
    vec3 NewColor = OldColor.rgb;
    float mid = NewColor.r+NewColor.g+NewColor.b;
    if (mid<1)
        NewColor=NewColor.rgb/mid;
    else if (mid<2)
        NewColor=NewColor.rgb/mid*2;
    else
        NewColor=NewColor.rgb/mid*3;
    
    NewColor.r=min(NewColor.r,1);
    NewColor.r=max(NewColor.r,0);
    NewColor.g=min(NewColor.g,1);
    NewColor.g=max(NewColor.g,0);
    NewColor.b=min(NewColor.b,1);
    NewColor.b=max(NewColor.b,0);

    return NewColor;
}
void main()
{
    if (Effect==1)
        color = vec4(MyEffect(texture(screenTexture, TexCoords)),1.0);
    else
        color = texture(screenTexture, TexCoords);
}