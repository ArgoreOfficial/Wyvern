#extension GL_ARB_bindless_texture : enable

#if GL_ES 
precision mediump float;
#endif

in vec2 TexCoord;
in vec3 Normal;
in vec3 Pos;
in flat int InstanceID;
in flat sampler2D Albedo;
in flat int HasAlpha;

layout(location = 0) out vec4 o_Albedo;
layout(location = 1) out vec2 o_Normal;
layout(location = 2) out vec2 o_RoughnessMetallic;

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

bool shouldDiscardAlpha( float _alpha )
{
    if( _alpha <= 0.01 )
        return true;

    if( _alpha < 1.0 )
    {
        float threshold_matrix[4][4] = {
            { 1.0  / 17.0,  9.0 / 17.0,  3.0 / 17.0, 11.0 / 17.0},
            { 13.0 / 17.0,  5.0 / 17.0, 15.0 / 17.0,  7.0 / 17.0},
            { 4.0  / 17.0, 12.0 / 17.0,  2.0 / 17.0, 10.0 / 17.0},
            { 16.0 / 17.0,  8.0 / 17.0, 14.0 / 17.0,  6.0 / 17.0}
        };

        if( _alpha - threshold_matrix[ int( gl_FragCoord.x ) % 4 ][ int( gl_FragCoord.y ) % 4 ] < 0.001 )
            return true;
        
    }

    return false;
}

vec2 OctWrap( vec2 v )
{
    vec2 w = 1.0 - abs( v.yx );
    if (v.x < 0.0) w.x = -w.x;
    if (v.y < 0.0) w.y = -w.y;
    return w;
}

vec2 encodeNormalOct( vec3 n )
{
    n /= ( abs( n.x ) + abs( n.y ) + abs( n.z ) );
    n.xy = n.z > 0.0 ? n.xy : OctWrap( n.xy );
    n.xy = n.xy * 0.5 + 0.5;
    return n.xy;
}

void main()
{
    vec3 normalColor = (Normal / 2.0) + vec3( 0.5 );
    
    // o_Albedo = randVec4( InstanceID + gl_PrimitiveID + 1 );
    vec4 color = texture( Albedo, TexCoord );

    if( HasAlpha == 1 && shouldDiscardAlpha( color.a ))
        discard;

    o_Albedo = color;
    o_Normal = encodeNormalOct( Normal );
    o_RoughnessMetallic = vec2( 1.0 );
}