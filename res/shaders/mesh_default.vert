#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 uProj;
uniform mat4 uView;
uniform mat4 uModel;

out vec2 TexCoord;

void main()
{
   gl_Position = uProj * uView * uModel * vec4(aPos, 1.0);
   TexCoord = aTexCoord;
   Color = uColor;
}
