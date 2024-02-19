#version 410 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedo;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
in vec4 Color;

uniform sampler2D uAlbedo;

void main()
{
    gPosition = vec4( FragPos, 1.0f );
    gNormal = vec4( normalize( Normal ), 1.0f );
    gAlbedo = texture( uAlbedo, TexCoord ) * Color;
}