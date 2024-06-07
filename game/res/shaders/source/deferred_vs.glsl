
layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec2 a_TexCoord;

out vec2 TexCoord;

void main()
{
    TexCoord = a_TexCoord;
    gl_Position = vec4( a_Pos, 1.0 );;
}