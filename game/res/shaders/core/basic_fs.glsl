#extension GL_ARB_bindless_texture : enable

#if GL_ES 
precision mediump float;
#endif

in vec2 TexCoord;
in vec3 Normal;
in vec3 Pos;
in flat int InstanceID;
in flat sampler2D Albedo;

layout(location = 0) out vec4 o_Albedo;
layout(location = 1) out vec4 o_Normal;
layout(location = 2) out vec4 o_RoughnessMetallic;

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
    
    // o_Albedo = randVec4( InstanceID + gl_PrimitiveID + 1 );
    o_Albedo = texture( Albedo, TexCoord );

    o_Normal = vec4( Normal, 1.0 );
    o_RoughnessMetallic = vec4( 1.0 );
}