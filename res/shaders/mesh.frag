#version 410 core

layout (location = 0) out vec4  gbuffer_Position;
layout (location = 1) out vec4  gbuffer_Normal;
layout (location = 2) out vec4  gbuffer_Albedo;
layout (location = 3) out vec4  gbuffer_MetallicRoughness;
layout (location = 4) out float gbuffer_Depth;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
in vec4 Color;

uniform sampler2D uAlbedo;
uniform sampler2D uMetallicRoughness;

void main()
{
    vec4 frag_color = texture( uAlbedo, TexCoord );

    if( frag_color.w < 0.1 )
        discard;
        
    gbuffer_Position          = vec4( FragPos, 1.0f );
    gbuffer_Normal            = vec4( normalize( Normal ), 1.0f );
    gbuffer_MetallicRoughness = texture( uMetallicRoughness, TexCoord );
    gbuffer_Depth             = gl_FragCoord.z;
    gbuffer_Albedo            = frag_color * Color;
    gbuffer_Albedo.w          = 1.0f;
}