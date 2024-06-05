#if GL_ES 
precision mediump float;
#endif

/// TODO: reflect to CPU so binding=0 doesn't need to be used
#if GL_ES 
uniform sampler2D u_TextureA;
uniform sampler2D u_TextureB;
#else
layout(binding = 0) uniform sampler2D u_Albedo;
layout(binding = 1) uniform sampler2D u_Normal;
layout(binding = 2) uniform sampler2D u_Position;
layout(binding = 3) uniform sampler2D u_RoughnessMetallic;
#endif

in vec2 TexCoord;
out vec4 FragColor;

const vec3 LIGHT_DIR = normalize( vec3( -0.5,1,0.9 ) );

void main()
{
    float over = (1.0 - floor( TexCoord.x )) * (1.0 - floor( TexCoord.y ));
    float x = TexCoord.x * over;
    float y = TexCoord.y * over;

    vec3 normal = texture( u_Normal, TexCoord ).xyz;
    float shading = 1.0;

    if( normal != vec3(0.0) )
        shading = max( 0.5, dot( normal, LIGHT_DIR ) );


    FragColor = texture( u_Albedo, TexCoord ) * shading;
}