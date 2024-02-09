#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

out vec4 color;

void main()
{
   gl_Position = proj * view * model * vec4(aPos, 1.0);
   color = aColor;
}