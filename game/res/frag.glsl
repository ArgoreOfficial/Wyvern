#if GL_ES 
precision mediump float;
#endif

uniform UbInput {
    vec3 u_Color;
    float u_Alpha;
};

layout(binding = 0) uniform sampler2D u_Texture;
layout(binding = 1) uniform sampler2D u_Texture2;

in vec2 TexCoord;

out vec4 FragColor;

void main()
{
    // FragColor = vec4( u_Color, u_Alpha );
    FragColor = texture( u_Texture, TexCoord ) + texture( u_Texture2, TexCoord );
}