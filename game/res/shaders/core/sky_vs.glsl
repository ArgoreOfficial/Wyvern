///////////////////////////////////////////////////////////////////////////////////////
// Common Shader Header
///////////////////////////////////////////////////////////////////////////////////////

struct sInstance
{
    mat4x4 Model;
};

struct sVertex
{
    float Pos[3];
    float Normal[3];
    float Tangent[3];
    float Color[4];
    float TexCoord0[2];
};

uniform UbInstanceData
{
    mat4x4 u_Projection;
    mat4x4 u_View;
    mat4x4 u_Model;
};

layout(std430) buffer SbInstances
{
    sInstance u_instances[];
};

layout(std430) buffer SbVertices
{
    sVertex u_vertices[];
};

/// TODO: move to a common shader header
/// TODO: shader preprocessor

vec3 getPosition( int _idx ) { 
    return vec3(
        u_vertices[ _idx ].Pos[0],
        u_vertices[ _idx ].Pos[1],
        u_vertices[ _idx ].Pos[2]
    );
}

vec3 getNormal( int _idx ) {
    return vec3(
        u_vertices[ _idx ].Normal[0],
        u_vertices[ _idx ].Normal[1],
        u_vertices[ _idx ].Normal[2]
    );
}

vec2 getTexCoord0( int _idx ) {
    return vec2(
        u_vertices[ _idx ].TexCoord0[0],
        u_vertices[ _idx ].TexCoord0[1]
    );
}

///////////////////////////////////////////////////////////////////////////////////////

out gl_PerVertex
{
    vec4 gl_Position;
};

out vec2 TexCoord;
out vec3 Normal;
out vec3 Pos;

void main()
{
    TexCoord = getTexCoord0( gl_VertexID );
    Normal = vec3( 0.0 );
    Pos = getPosition( gl_VertexID );

    vec4 p = u_Projection * mat4x4( mat3x3( u_View ) )* u_Model * vec4( Pos, 1.0 );
    gl_Position = p.xyww;
}