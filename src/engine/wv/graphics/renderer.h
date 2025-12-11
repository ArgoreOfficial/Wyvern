#pragma once

#include <stdint.h>
#include <glad/glad.h>

#include <wv/graphics/vertex.h>
#include <wv/graphics/shader.h>
#include <wv/graphics/texture.h>

#include <wv/math/vector2.h>
#include <wv/helpers/unordered_array.hpp>
#include <wv/math/matrix.h>

namespace wv {

struct VertexData
{
	wv::Vector3f normal;
	wv::Vector3f color;
	wv::Vector2f texCoord;
};

struct MeshSurface
{
	std::vector<wv::Vector3f> positions;
	std::vector<wv::VertexData> datas;

	void addPosition( const wv::Vector3f& _position ) {
		if ( positions.size() != datas.size() )
			datas.push_back( {} );

		positions.push_back( _position );
	}

	void addData( const wv::Vector3f& _normal = {}, const wv::Vector3f& _color = {}, const wv::Vector2f& _texcoord = {} ) {
		if ( positions.size() - 1 != datas.size() )
			return;

		VertexData data;
		data.normal   = _normal;
		data.color    = _color;
		data.texCoord = _texcoord;
		datas.push_back( data );
	}
};

struct GLRenderMesh
{
	GLStorageBuffer positionBuffer;
	GLStorageBuffer vertexDataBuffer;
	size_t numVertices;
};

class OpenGLRenderer
{
public:
	bool setup();
	void shutdown();

	void prepare( uint32_t _width, uint32_t _height );
	void finalize();

	void clear( float _r, float _g, float _b, float _a );

	void draw( int _first, uint32_t _count );

	ResourceID createRenderMesh( const MeshSurface& _meshSurface );
	void destroyRenderMesh( ResourceID _handle );
	void drawRenderMesh( ResourceID _handle );

	ResourceID createPipeline( const char* _vert_src, const char* _frag_src );
	void destroyPipeline( ResourceID _handle );
	void bindPipeline( ResourceID _handle );

	ResourceID createTexture( unsigned char* _data, uint32_t _width, uint32_t _height, uint32_t _channels, TextureFormat _format, bool _generate_mips, TextureFiltering _filtering );
	void destroyTexture( ResourceID _handle );
	void bindTexture( ResourceID _handle, uint32_t _slot );

	Vector2i getTextureSize( ResourceID _handle );

	void setVSUniformMatrix4x4( ResourceID _handle, uint32_t _location, const wv::Matrix4x4f& _matrix );
	void setFSUniformMatrix4x4( ResourceID _handle, uint32_t _location, const wv::Matrix4x4f& _matrix );

	void setVSUniformVector2f( ResourceID _pipeline, uint32_t _location, const wv::Vector2f& _vector );

private:

	GLStorageBuffer createStorageBuffer( void* _data, size_t _data_size );
	void destroyStorageBuffer( const GLStorageBuffer& _buffer );
	void bindStorageBufferToSlot( const GLStorageBuffer& _buffer, int _slot );

	GLuint compileShaderModule( const char* _source, GLenum _type );

	wv::unordered_array<ResourceID, GLShaderPipeline> m_pipelines;
	wv::unordered_array<ResourceID, GLTexture> m_textures;

	wv::unordered_array<ResourceID, GLRenderMesh> m_renderMeshes;
	
	GLuint m_empty_vao = 0;
};


}