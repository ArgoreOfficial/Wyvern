#if GL_ES 
in vec3 a_Pos;
in vec2 a_TexCoord;
#else
layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec2 a_TexCoord;
#endif

uniform UbInstanceData
{
    mat4x4 u_Projection;
    mat4x4 u_View;
    mat4x4 u_Model;
};

out vec2 TexCoord;

void main()
{
    gl_Position = u_Projection * u_View * u_Model * vec4( a_Pos, 1.0 );
    TexCoord = a_TexCoord;
}