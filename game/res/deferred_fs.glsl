#if GL_ES 
precision mediump float;
#endif

/// TODO: reflect to CPU so binding=0 doesn't need to be used
#if GL_ES 
uniform sampler2D u_TextureA;
uniform sampler2D u_TextureB;
#else
layout(binding = 0) uniform sampler2D u_TextureA;
layout(binding = 1) uniform sampler2D u_TextureB;
#endif

in vec2 TexCoord;

out vec4 FragColor;

void main()
{
    float over = (1.0 - floor( TexCoord.x )) * (1.0 - floor( TexCoord.y ));
    float x = TexCoord.x * over;
    float y = TexCoord.y * over;

    FragColor = texture( u_TextureA, TexCoord ) * texture( u_TextureB, TexCoord );
}