#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;

uniform mat4 transform;

out vec3 frag;

void main()
{
   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
   frag = aCol;
}