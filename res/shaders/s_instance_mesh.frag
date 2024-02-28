#version 430 core

layout (location = 0) out vec4  gbuffer_Position;
layout (location = 1) out vec4  gbuffer_Normal;
layout (location = 2) out vec4  gbuffer_Albedo;
layout (location = 3) out vec4  gbuffer_MetallicRoughness;
layout (location = 4) out float gbuffer_Depth;

layout(binding = 0) uniform sampler2D uAlbedo;
layout(binding = 1) uniform sampler2D uMetallicRoughness;
layout(binding = 2) uniform sampler2D uNormal;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
in vec4 Color;
in mat3 TBN;

void main()
{
    vec4 frag_color = texture( uAlbedo, TexCoord );

    if( frag_color.w < 0.1 )
        discard;
    
    vec3 normal = texture( uNormal, TexCoord ).rgb;
    normal = normal * 2.0 - 1.0;   
    normal = normalize( TBN * normal ); 
    
    gbuffer_Normal = vec4( normal, 1.0f );
    
    gbuffer_Position          = vec4( FragPos, 1.0f );
    gbuffer_MetallicRoughness = texture( uMetallicRoughness, TexCoord );
    gbuffer_Depth             = gl_FragCoord.z;
    gbuffer_Albedo            = vec4( (frag_color * Color).rgb, 1.0f );
}