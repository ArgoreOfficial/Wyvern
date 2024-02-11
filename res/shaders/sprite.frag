#version 410 core

out vec4 FragColor;

in vec2 TexCoord;
in vec4 Color;

uniform sampler2D texture_sampler;

void main()
{
    FragColor = texture(texture_sampler, TexCoord) * Color;
}