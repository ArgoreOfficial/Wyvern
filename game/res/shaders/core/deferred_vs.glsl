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

void main()
{
    TexCoord = getTexCoord0( gl_VertexID );
    gl_Position = vec4( getPosition( gl_VertexID ), 1.0 );
}