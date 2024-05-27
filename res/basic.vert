#version 330 core

uniform vec3 u_Col;

in vec3 a_Pos;

out vec4 outColor;

void main()
{
    gl_Position = vec4( a_Pos, 1.0f );
    outColor = vec4( u_Col, 1.0f );
}