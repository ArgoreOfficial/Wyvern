#if GL_ES 
precision mediump float;
#endif

/// TODO: reflect to CPU so binding=0 doesn't need to be used
#if GL_ES 
uniform sampler2D u_Albedo;
uniform sampler2D u_Normal;
uniform sampler2D u_Position;
uniform sampler2D u_RoughnessMetallic;
#else
layout(binding = 0) uniform sampler2D u_Albedo;
layout(binding = 1) uniform sampler2D u_Normal;
layout(binding = 2) uniform sampler2D u_Position;
layout(binding = 3) uniform sampler2D u_RoughnessMetallic;
#endif

in vec2 TexCoord;
out vec4 FragColor;

const vec3 LIGHT_DIR = normalize( vec3( 1,1,-1 ) );

void main()
{
    float over = (1.0 - floor( TexCoord.x )) * (1.0 - floor( TexCoord.y ));
    float x = TexCoord.x * over;
    float y = TexCoord.y * over;

    vec3 normal = texture( u_Normal, TexCoord ).xyz;
    float shading = 1.0;

    if( length( normal ) > 0.1 )
    {
        float shadingDot = dot( normal, LIGHT_DIR );
        shading = max( 0.0, shadingDot * 0.5 + 0.5 );
    }

    FragColor = vec4( texture( u_Albedo, TexCoord ).rgb * shading, 1.0 );
}