#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec4 aVertexColor;
layout (location = 4) in vec2 aTexCoord;

layout (std140) uniform uboInstanceTransform
{
   mat4 uProj;
   mat4 uView;
   mat4 uModel;
   vec4 uColor;

   mat4 uInstanceMatrices[ 512 ];
};

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;
out vec4 Color;
out mat3 TBN;

void main()
{
   mat4 model = uInstanceMatrices[ gl_InstanceID ];
   gl_Position = uProj * uView * model * vec4( aPos, 1.0 );
   
   vec3 T = normalize( vec3( model * vec4( aTangent, 0.0f ) ) );
   vec3 N = normalize( vec3( model * vec4( aNormal,  0.0f ) ) );
   vec3 B = cross( N, T );

   TBN = mat3(T, B, N);
   
   TexCoord = aTexCoord;
   FragPos  = (model * vec4( aPos, 1.0 )).xyz;
   Normal   = aNormal;
   Color    = uColor * aVertexColor;
}
