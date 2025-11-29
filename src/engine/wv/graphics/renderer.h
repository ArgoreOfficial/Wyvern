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

class OpenGLRenderer
{
public:
	bool setup( GLADloadproc _load_proc );
	void shutdown();

	void prepare( uint32_t _width, uint32_t _height );
	void finalize();

	void clear( float _r, float _g, float _b, float _a );

	void draw( int _first, uint32_t _count );

	ResourceID createPipeline( const char* _vert_src, const char* _frag_src );
	void destroyPipeline( ResourceID _handle );
	void bindPipeline( ResourceID _handle );

	ResourceID createVertexBuffer( Vertex* _data, size_t _data_size );
	void destroyVertexBuffer( ResourceID _handle );
	void bindVertexBuffer( ResourceID _handle );

	ResourceID createTexture( unsigned char* _data, uint32_t _width, uint32_t _height, uint32_t _channels, TextureFormat _format, bool _generate_mips, TextureFiltering _filtering );
	void destroyTexture( ResourceID _handle );
	void bindTexture( ResourceID _handle, uint32_t _slot );

	Vector2i getTextureSize( ResourceID _handle );

	void setVSUniformMatrix4x4( ResourceID _handle, uint32_t _location, const wv::Matrix4x4f& _matrix );
	void setFSUniformMatrix4x4( ResourceID _handle, uint32_t _location, const wv::Matrix4x4f& _matrix );

	void setVSUniformVector2f( ResourceID _pipeline, uint32_t _location, const wv::Vector2f& _vector );

private:

	GLuint compileShaderModule( const char* _source, GLenum _type );

	wv::unordered_array<ResourceID, GLShaderPipeline> m_pipelines;
	wv::unordered_array<ResourceID, GLVertexBuffer> m_vertex_buffers;
	wv::unordered_array<ResourceID, GLTexture> m_textures;
	
	GLuint m_empty_vao = 0;
};


}