#version 410 core

out vec4 FragColor;

in vec2 TexCoord;
in vec4 Color;

uniform sampler2D uAlbedo;

void main()
{
    FragColor = texture( uAlbedo, TexCoord ) * Color;
}