
layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec4 a_Color;
layout(location = 4) in vec2 a_TexCoord0;

uniform UbCameraData
{
    mat4x4 u_Projection;
    mat4x4 u_View;
    mat4x4 u_Model;
};

out gl_PerVertex
{
    vec4 gl_Position;
};

out vec2 TexCoord;
out vec3 Normal;
out vec3 Pos;

void main()
{
    TexCoord = a_TexCoord0;
    Normal = vec3( 0.0 );
    Pos = a_Pos;

    gl_Position = u_Projection * u_View * u_Model * vec4( a_Pos, 1.0 );
}