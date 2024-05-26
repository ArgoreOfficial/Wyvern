#version 330 core

in vec3 a_Pos;

out vec4 outColor;

void main()
{
    gl_Position = vec4( a_Pos, 1.0f );
    outColor = vec4( 0.8f, 0.1f, 0.1f, 1.0f );
}