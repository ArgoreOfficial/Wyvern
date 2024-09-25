
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
out flat int InstanceID;

///////////////////////////////////////////////////////////////////////////////////////

void main()
{
    mat4x4 model = u_instances[ gl_InstanceID ].Model;
    
    InstanceID = gl_InstanceID;

    Pos = getPosition( gl_VertexID );
    TexCoord = getTexCoord0( gl_VertexID );
    Normal = getNormal( gl_VertexID );
    Normal = normalize( transpose( inverse( mat3( model ) ) ) * Normal );
    gl_Position = u_Projection * u_View * model * vec4( Pos, 1.0 );
}