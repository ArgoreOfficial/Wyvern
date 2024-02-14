#version 410 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D uAlbedo;

void main()
{
    FragColor = texture( uAlbedo, TexCoord );
}