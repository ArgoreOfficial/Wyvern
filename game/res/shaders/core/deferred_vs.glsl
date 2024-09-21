
layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec4 a_Color;
layout(location = 4) in vec2 a_TexCoord0;

out gl_PerVertex
{
    vec4 gl_Position;
};

out vec2 TexCoord;

void main()
{
    TexCoord = a_TexCoord0;
    gl_Position = vec4( a_Pos, 1.0 );;
}