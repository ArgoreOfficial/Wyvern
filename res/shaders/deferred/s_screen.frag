#version 410 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;

layout (std140) uniform uboRenderInfo
{
   int uRenderMode;
};

void main()
{
    vec4 light_dir = vec4( normalize( vec3( 1.0f, -1.0f, 0.5f ) ), 1.0f );

    vec4 normal = texture( gNormal, TexCoord );
    vec4 position = texture( gPosition, TexCoord );
    vec4 albedo = texture( gAlbedo, TexCoord );
    
    float directional = dot( light_dir, normal ) * 0.5f + 0.5f;
    float ambient = 0.2f;

    if( uRenderMode == 1 ) FragColor = albedo * ( ambient + directional * (1.0f-ambient) );
    else if( uRenderMode == 2 ) FragColor = normal;
    else if( uRenderMode == 3 ) FragColor = position;
    else if( uRenderMode == 4 ) FragColor = albedo;
    else if( uRenderMode == 5 ) FragColor = vec4( 1.0f, 1.0f, 1.0f, 1.0f ) * directional;
}