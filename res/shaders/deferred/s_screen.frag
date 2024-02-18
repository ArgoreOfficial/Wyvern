#version 410 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;

void main()
{
    vec2 tex = TexCoord * 2;

    if( TexCoord.x < 0.5f )
    {
        if( TexCoord.y < 0.5f ) FragColor = texture( gPosition, tex );
        else                    FragColor = texture( gNormal, tex );
    }
    else
    {
        if( TexCoord.y > 0.5f ) FragColor = texture( gAlbedo, tex );
        else discard;
    }
}