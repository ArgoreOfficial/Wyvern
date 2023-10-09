#shader vertex
#version 330 core

layout( location = 0 ) in vec3 position;
layout( location = 1 ) in vec2 texCoord;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

out vec2 v_TexCoord;

void main()
{
	gl_Position = u_Projection * u_View * u_Model * vec4( position, 1.0 );
	v_TexCoord = texCoord;
}


#shader fragment
#version 330 core

out vec4 color;
in vec2 v_TexCoord;

uniform vec4 u_Color;
uniform sampler2D u_Texture;

void main()
{
	vec4 texColor = texture( u_Texture, v_TexCoord );
	color = texColor;
}