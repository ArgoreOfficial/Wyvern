#if GL_ES 
in vec3 a_Pos;
#else
layout(location = 0) in vec3 a_Pos;
#endif

void main()
{
    gl_Position = vec4( a_Pos, 1.0 );
}