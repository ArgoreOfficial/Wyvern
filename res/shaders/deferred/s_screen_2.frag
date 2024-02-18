#version 410 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D gGamer;

void main()
{
    vec2 tex = TexCoord * 2;

    if( TexCoord.x > 0.5f && TexCoord.y < 0.5f )
    {
        FragColor = texture( gGamer, tex );
    }

}