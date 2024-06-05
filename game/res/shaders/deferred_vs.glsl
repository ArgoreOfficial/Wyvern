#if GL_ES 
in vec3 a_Pos;
in vec2 a_TexCoord;
#else
layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec2 a_TexCoord;
#endif

out vec2 TexCoord;

void main()
{
    gl_Position = vec4( a_Pos, 1.0 );
    TexCoord = a_TexCoord;
}