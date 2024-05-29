#if GL_ES 
in vec3 a_Pos;
#else
layout(location = 0) in vec3 a_Pos;
#endif

uniform UbInstanceData
{
    mat4x4 u_Projection;
    mat4x4 u_View;
    mat4x4 u_Model;
};

void main()
{
    gl_Position = u_Projection * u_View * u_Model * vec4( a_Pos, 1.0 );
}