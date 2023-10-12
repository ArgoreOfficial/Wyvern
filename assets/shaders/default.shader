#shader vertex
#version 330 core

layout( location = 0 ) in vec3 aPos;
layout( location = 1 ) in vec2 aUV;
layout( location = 2 ) in vec3 aNormal;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;
uniform vec2 u_UVOffset;
uniform vec2 u_tileCount;

out vec2 v_TexCoord;
out vec3 v_Normal;

void main()
{
	gl_Position = u_Projection * u_View * u_Model * vec4( aPos, 1.0 );
	v_TexCoord = ( aUV + u_UVOffset ) / u_tileCount;
	
	vec4 transformedNormal = u_Model * vec4( aNormal, 1.0 );
	v_Normal = normalize( vec3( transformedNormal ) );
}


#shader fragment
#version 330 core

out vec4 color;
in vec2 v_TexCoord;
in vec3 v_Normal; 

uniform vec4 u_Color;
uniform int u_hasTexture = 0;
uniform sampler2D u_Texture;

void main()
{
	vec4 texColor = u_Color;
	if ( u_hasTexture == 1 )
	{
		texColor = texture( u_Texture, v_TexCoord );
	}
	color = texColor * max(v_Normal.z, 0.2);
}