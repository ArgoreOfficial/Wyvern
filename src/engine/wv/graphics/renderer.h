#pragma once

#include <stdint.h>
#include <glad/glad.h>

#include <wv/graphics/vertex.h>
#include <wv/graphics/shader.h>
#include <wv/graphics/texture.h>

#include <wv/math/vector2.h>
#include <wv/helpers/unordered_array.hpp>
#include <wv/math/matrix.h>
#include <wv/math/line.h>

namespace wv {

class World;

struct GLRenderMaterial
{
	GLuint shaderProgram = 0;
	GLuint vertModule = 0;
	GLuint fragModule = 0;

	size_t dataBufferSize;
	GLuint sceneDataSlot    = GL_INVALID_INDEX;
	GLuint materialDataSlot = GL_INVALID_INDEX;
};

struct GLRenderMesh
{
	GLStorageBuffer positionBuffer;
	GLStorageBuffer extraVertexDataBuffer;
	size_t numVertices = 0;
	bool hasExtraVertexData = false;
	
	GLuint indexBuffer = 0;
	size_t numIndices = 0;

	ResourceID material;
	GLStorageBuffer materialDataBuffer;
};

struct SceneData
{
	wv::Matrix4x4f viewProj;
};

struct MaterialData
{
	wv::Matrix4x4f model;
};

struct RenderView
{
	SceneData sceneData;
	std::vector<ResourceID> renderMeshes;
};

class OpenGLRenderer
{
public:
	bool setup();
	void shutdown();

	void setupDebug( const char* _vertexShaderSource, const char* _fragmentShaderSource );

	void prepare( uint32_t _width, uint32_t _height );
	void finalize();

	void clearColor( float _r, float _g, float _b, float _a );
	void clearDepth( double _v = 1.0 );

	ResourceID createMaterial();
	void destroyMaterial( ResourceID _handle );
	void setMaterialVertexShader( ResourceID _handle, const char* _src );
	void setMaterialFragmentShader( ResourceID _handle, const char* _src );
	void finalizeMaterial( ResourceID _handle );

	ResourceID createRenderMesh( wv::Vector3f* _positions, size_t _numPositions, void* _extraVertexData = nullptr, size_t _sizeExtraVertexData = 0 );
	ResourceID createRenderMesh( wv::Vector3f* _positions, size_t _numPositions, uint16_t* _indices, size_t _numIndices, void* _extraVertexData = nullptr, size_t _sizeExtraVertexData = 0 );
	void destroyRenderMesh( ResourceID _handle );
	void setRenderMeshMaterial( ResourceID _meshHandle, ResourceID _materialHandle );

	void drawRenderView( const RenderView& _renderView );
	void drawDebugLines( const std::vector<wv::Line3f>& _lines );

	void renderWorld( World* _world );

	ResourceID createTexture( unsigned char* _data, uint32_t _width, uint32_t _height, uint32_t _channels, TextureFormat _format, bool _generate_mips, TextureFiltering _filtering );
	void destroyTexture( ResourceID _handle );
	void bindTexture( ResourceID _handle, uint32_t _slot );
	Vector2i getTextureSize( ResourceID _handle );

private:

	GLStorageBuffer createStorageBuffer( void* _data, size_t _dataSize );
	void destroyStorageBuffer( const GLStorageBuffer& _buffer );
	void bindStorageBufferToSlot( const GLStorageBuffer& _buffer, int _slot );
	void uploadStorageBuffer( const GLStorageBuffer& _buffer, void* _data, size_t _dataSize, size_t _offset = 0 );

	GLuint compileShaderModule( const char* _source, GLenum _type );

	wv::unordered_array<ResourceID, GLShaderPipeline> m_pipelines;
	wv::unordered_array<ResourceID, GLTexture>        m_textures;
	wv::unordered_array<ResourceID, GLRenderMesh>     m_renderMeshes;
	wv::unordered_array<ResourceID, GLRenderMaterial> m_renderMaterials;
	
	GLuint m_empty_vao = 0;
	GLuint m_uboSceneDataBlock = 0;
	
	const GLuint m_sceneDataBindPoint  = 0;
	const GLuint m_materialDataBindPoint = 1;

	ResourceID m_debugLineMaterial = 0;
	GLStorageBuffer m_debugLineVertexBuffer{};
	GLStorageBuffer m_debugLineMaterialBuffer{};

};


}