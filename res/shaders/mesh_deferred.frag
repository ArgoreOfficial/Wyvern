#version 410 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D uAlbedo;

void main()
{
    gPosition = vec4( FragPos, 1.0f );
    gNormal = vec4( normalize( Normal ), 1.0f );
    gAlbedoSpec.rgb = texture( uAlbedo, TexCoord ).rgb;          // albedo 
    gAlbedoSpec.a = 1.0f; // texture( uSpecular, TexCoord ).r;   // specular
}