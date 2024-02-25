#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec4 aVertexColor;
layout (location = 3) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
   gl_Position = vec4( aPos, 1.0 );
   
   TexCoord = vec2( aTexCoord.x, 1.0f - aTexCoord.y );
}
