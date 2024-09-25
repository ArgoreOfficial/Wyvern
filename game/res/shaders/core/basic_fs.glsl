#if GL_ES 
precision mediump float;
#endif

/// TODO: reflect to CPU so binding=0 doesn't need to be used
#if GL_ES 
uniform sampler2D u_Albedo;
#else
layout(binding = 0) uniform sampler2D u_Albedo;
#endif

in vec2 TexCoord;
in vec3 Normal;
in vec3 Pos;
in flat int InstanceID;

layout(location = 0) out vec4 o_Albedo;
layout(location = 1) out vec4 o_Normal;
layout(location = 2) out vec4 o_Position;
layout(location = 3) out vec4 o_RoughnessMetallic;

float randf(vec2 p)
{
	return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}

vec4 randVec4( int _id )
{
    return vec4(
        randf( vec2( _id,   0 ) ),
        randf( vec2(   0, _id ) ),
        randf( vec2( _id, _id ) ),
        1.0
    );
}

void main()
{
    vec3 normalColor = (Normal / 2.0) + vec3( 0.5 );

    o_Albedo = texture( u_Albedo, TexCoord );
    //o_Albedo = randVec4( InstanceID + gl_PrimitiveID + 1 );
    
    o_Normal = vec4( Normal, 1.0 );
    o_Position = vec4( Pos, 1.0 );
    o_RoughnessMetallic = vec4( 1.0 );
}