#version 410 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedo;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
in vec4 Color;
in vec3 ScreenNormal;

uniform sampler2D uAlbedo;

void main()
{
    vec4 frag_color = texture( uAlbedo, TexCoord );

    // Screen-door transparency: Discard pixel if below threshold.
    float threshold = 0.5;

    if(frag_color.w < threshold)
    {
        mat4 thresholdMatrix;
        thresholdMatrix[ 0 ] = vec4( 1.0  / 17.0,  9.0 / 17.0,  3.0 / 17.0, 11.0 / 17.0 );
        thresholdMatrix[ 1 ] = vec4( 13.0 / 17.0,  5.0 / 17.0, 15.0 / 17.0,  7.0 / 17.0 );
        thresholdMatrix[ 2 ] = vec4( 4.0  / 17.0, 12.0 / 17.0,  2.0 / 17.0, 10.0 / 17.0 );
        thresholdMatrix[ 3 ] = vec4( 16.0 / 17.0,  8.0 / 17.0, 14.0 / 17.0,  6.0 / 17.0 );

        if( frag_color.w < thresholdMatrix[ int(mod(gl_FragCoord.x, 4)) ][ int(mod(gl_FragCoord.y, 4)) ] )
            discard;
    }

    gPosition = vec4( FragPos, 1.0f );
    gNormal = vec4( normalize( Normal ), 1.0f );
    
    gAlbedo = frag_color * Color;
    gAlbedo.w = 1.0f;
}