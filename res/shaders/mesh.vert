#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec4 aVertexColor;
layout (location = 3) in vec2 aTexCoord;

layout (std140) uniform uboTransform
{
   mat4 uProj;
   mat4 uView;
   mat4 uModel;
   vec4 uColor;
};

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;
out vec4 Color;
out vec3 ScreenNormal;

void main()
{
   gl_Position = uProj * uView * uModel * vec4( aPos, 1.0 );
   
   TexCoord = aTexCoord;
   FragPos  = aPos;
   Normal   = aNormal;
   Color    = uColor * aVertexColor;

   vec3 snormal = (uView * uModel * vec4( aNormal, 1.0 )).xyz;
   ScreenNormal = normalize( snormal );
}
