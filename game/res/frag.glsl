#if GL_ES 
precision mediump float;
#endif

uniform UbInput {
    vec3 u_Color;
    float u_Alpha;
};

/// TODO: reflect to CPU so binding=0 doesn't need to be used
#if GL_ES 
uniform sampler2D u_Texture;
#else
layout(binding = 0) uniform sampler2D u_Texture;
#endif

in vec2 TexCoord;

out vec4 FragColor;

void main()
{
    FragColor = texture( u_Texture, TexCoord );
}