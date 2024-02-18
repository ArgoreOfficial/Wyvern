#version 410 core

layout (location = 0) out vec4 gGamer;

in vec2 TexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;

void main()
{
    gGamer = texture( gPosition, TexCoord ) * texture( gAlbedo, TexCoord );
}