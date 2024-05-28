#if GL_ES 
precision mediump float;
#endif

uniform UbInput {
    vec3 u_Color;
    float u_Alpha;
};

out vec4 FragColor;

void main()
{
    FragColor = vec4( u_Color, u_Alpha );
} 