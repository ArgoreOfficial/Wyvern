#if GL_ES 
in vec3 a_Pos;
in vec3 a_Normal;
in vec3 a_Tangent;
in vec4 a_Color;
in vec2 a_TexCoord0;
#else
layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec4 a_Color;
layout(location = 4) in vec2 a_TexCoord0;
#endif

uniform UbInstanceData
{
    mat4x4 u_Projection;
    mat4x4 u_View;
    mat4x4 u_Model;
};

out vec2 TexCoord;
out vec3 Normal;
out vec3 Pos;

void main()
{
    gl_Position = u_Projection * u_View * u_Model * vec4( a_Pos, 1.0 );
    TexCoord = a_TexCoord0;
    Normal = a_Normal;
    Pos = a_Pos;
}