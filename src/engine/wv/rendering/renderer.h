#pragma once


#include <wv/debug/log.h>

#include <wv/helpers/unordered_array.hpp>
#include <wv/math/vector2.h>
#include <wv/math/matrix.h>
#include <wv/math/vector3.h>
#include <wv/math/matrix.h>

#include <wv/rendering/command_buffer.h>
#include <wv/rendering/pipeline_manager.h>
#include <wv/rendering/image_manager.h>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <functional>
#include <stdint.h>
#include <vector>
#include <span>
#include <wv/resource_id.h>

namespace wv {

class World;

struct SceneData
{
	wv::Matrix4x4f viewProj;
};

struct MaterialData
{
	wv::Matrix4x4f model;
};

struct FrameData
{
	VkCommandPool commandPool;
	CommandBuffer* mainCommandBuffer = nullptr;

	VkSemaphore acquireSemaphore;
	VkFence fence;
};

struct DeleteQueue
{
	std::vector<std::function<void()>> deleteQueue;

	void push( const std::function<void()>& _func ) { deleteQueue.push_back( _func ); }

	void flush() {
		for ( auto it = deleteQueue.rbegin(); it != deleteQueue.rend(); it++ )
			( *it )( );
		deleteQueue.clear();
	}
};

constexpr uint32_t FRAME_OVERLAP = 2;

struct AllocatedBuffer
{
	VkBuffer buffer;
	VmaAllocation allocation;
	VmaAllocationInfo info;
};

struct GPUMeshBuffers
{
	AllocatedBuffer indexBuffer;
	AllocatedBuffer positionBuffer;
	AllocatedBuffer vertexDataBuffer;

	VkDeviceAddress positionBufferAddress;
	VkDeviceAddress vertexDataBufferAddress;

	uint32_t numIndices;
};

struct GPUDrawPushConstants
{
	wv::Matrix4x4f worldMatrix;
	VkDeviceAddress positionBuffer;
	VkDeviceAddress vertexDataBuffer;
};

enum UniformType
{
	UNIFORM_TYPE_FLOAT,
	UNIFORM_TYPE_VEC2,
	UNIFORM_TYPE_VEC3,
	UNIFORM_TYPE_TEXTURE,
	UNIFORM_TYPE_BUFFER_ADDRESS,
	UNIFORM_TYPE_MATRIX,
};

class MaterialType;

struct MaterialInstance
{
	MaterialType* type;

	std::vector<uint8_t> buffer;
};

class MaterialType
{
public:
	struct UniformSpan
	{
		std::string name;
		UniformType type;
		uint32_t offset;
	};

	void addSpan( const std::string& _name, UniformType _type ) {
		if ( m_uniformNameMap.contains( _name ) )
		{
			WV_LOG_ERROR( "Uniform %s already exists\n", _name.c_str() );
			return;
		}

		uint32_t offset = 0; 

		if ( m_uniforms.size() > 0 )
		{
			offset += m_uniforms.back().offset;
			offset += uniformTypeSize( m_uniforms.back().type );
		}

		m_uniformNameMap[ _name ] = m_uniforms.size();
		m_uniforms.push_back( UniformSpan{ _name, _type, offset } );
	}

	MaterialInstance createInstance() {
		size_t size = 0;
		for ( auto span : m_uniforms )
			size += uniformTypeSize( span.type );

		MaterialInstance instance{};
		instance.type = this;
		instance.buffer.resize( size );
		return instance;
	}

	template<typename Ty>
	void setValue( MaterialInstance& _instance, const std::string& _name, const Ty& _value ) {
		if ( !m_uniformNameMap.contains( _name ) )
		{
			WV_LOG_ERROR( "Uniform '%s' does not exist\n", _name.c_str() );
			return;
		}

		UniformSpan span = m_uniforms[ m_uniformNameMap.at( _name ) ];
		if ( sizeof( Ty ) != uniformTypeSize( span.type ) )
		{
			WV_LOG_ERROR( "Uniform '%s' size error\n", _name.c_str() );
			return;
		}

		if ( span.offset + sizeof( Ty ) > _instance.buffer.size() )
		{
			WV_LOG_ERROR( "Uniform '%s' out of bounds\n", _name.c_str() );
			return;
		}

		std::memcpy( &_instance.buffer[ span.offset ], &_value, sizeof( Ty ) );
	}

private:
	uint32_t uniformTypeSize( UniformType _type ) {
		switch ( _type )
		{
		case UNIFORM_TYPE_FLOAT: return sizeof( float ); break;
		case UNIFORM_TYPE_VEC2: return sizeof( Vector2f ); break;
		case UNIFORM_TYPE_VEC3: return sizeof( Vector3f ); break;
		case UNIFORM_TYPE_TEXTURE: return sizeof( uint32_t ); break;
		}

		return 0;
	}

	size_t m_bufferSize = 0;
	std::vector<UniformSpan> m_uniforms;
	std::unordered_map<std::string, size_t> m_uniformNameMap;
};


class Renderer
{
	friend class Application;
	friend class PipelineManager;
	friend class ImageManager;

public:
	bool initialize();
	void shutdown();

	void prepare( uint32_t _width, uint32_t _height );
	void render( World* _world );
	void finalize();

	bool isSwapchainOutOfDate() const { return m_resizeRequested; }

	ResourceID createMesh( const std::vector<uint16_t>& _indices, const std::vector<Vector3f>& _vertexPositions, void* _vertexData = nullptr, size_t _vertexDataSize = 0 );
	void destroyMesh( ResourceID _mesh );

protected:
	void waitForRenderer() const { vkDeviceWaitIdle( m_device ); }
	
	FrameData& getCurrentFrame() { return m_frames[ m_frameNumber % FRAME_OVERLAP ]; };

	bool initVulkan();
	bool initSwapchain( uint32_t _width, uint32_t _height );
	bool initCommands();
	bool initSyncStructures();
	bool initDescriptors();

	void createSwapchain( uint32_t _width, uint32_t _height );
	void destroySwapchain();
	void resizeSwapchain( uint32_t _width, uint32_t _height );

	void drawBackground( CommandBuffer* _cmd );
	void drawGeometry( CommandBuffer* _cmd, World* _world );

	void immediateCmdSubmit( std::function<void( CommandBuffer& _cmd )>&& _func );

	AllocatedBuffer createBuffer( size_t _size, VkBufferUsageFlags _usage, VmaMemoryUsage _memoryUsage );
	void destroyBuffer( const AllocatedBuffer& _buffer );

	void storeImage( ImageID _imageID, VkSampler _sampler, uint32_t _at );

	const bool m_useValidationLayers = true;

	bool m_initialized = false;
	bool m_resizeRequested = false;

	PipelineManager m_pipelineManager = { this };
	ImageManager m_imageManager = { this };

	DeleteQueue m_mainDeleteQueue = {};

	VkInstance               m_instance       = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
	VkPhysicalDevice         m_physicalDevice = VK_NULL_HANDLE;
	VkDevice                 m_device         = VK_NULL_HANDLE;
	VkSurfaceKHR             m_surface        = VK_NULL_HANDLE;

	VkSwapchainKHR m_swapchain            = VK_NULL_HANDLE;
	VkFormat       m_swapchainImageFormat = VK_FORMAT_UNDEFINED;

	std::vector<VkImage>     m_swapchainImages     = {};
	std::vector<VkImageView> m_swapchainImageViews = {};
	std::vector<VkSemaphore> m_submitSemaphores    = {};
	VkExtent2D m_swapchainExtent = {};

	ImageID m_drawImage  = {};
	ImageID m_depthImage = {};
	VkExtent2D m_drawExtent = {};

	uint32_t  m_frameNumber = 0;
	FrameData m_frames[ FRAME_OVERLAP ];

	VkQueue  m_graphicsQueue = VK_NULL_HANDLE;
	uint32_t m_graphicsQueueFamily = 0;

	VmaAllocator m_allocator = VK_NULL_HANDLE;

	VkFence        m_immediateFence         = VK_NULL_HANDLE;
	VkCommandPool  m_immediateCommandPool   = VK_NULL_HANDLE;
	CommandBuffer* m_immediateCommandBuffer = nullptr;

	// Bindless

	const uint32_t STORAGE_BINDING = 0;
	const uint32_t SAMPLER_BINDING = 1;
	const uint32_t IMAGE_BINDING   = 2;
	
	const uint32_t STORAGE_COUNT = 65536;
	const uint32_t SAMPLER_COUNT = 65536;
	const uint32_t IMAGE_COUNT   = 65536;

	VkDescriptorPool      m_bindlessPool;
	VkDescriptorSetLayout m_bindlessLayout;
	VkDescriptorSet       m_bindlessDescriptorSet;
	VkPipelineLayout      m_bindlessPipelineLayout;

	// Debug Images
	
	VkSampler m_samplerLinear  = VK_NULL_HANDLE;
	VkSampler m_samplerNearest = VK_NULL_HANDLE;

	ImageID m_blackImage{};
	ImageID m_whiteImage{};
	ImageID m_debugImage{};
	
	// TESTING STUFF

	MaterialType m_triangleMaterialType = {};
	MaterialInstance m_triangleMaterialInstance = {};

	PipelineID m_trianglePipelineID = {};
	
	unordered_array<ResourceID, GPUMeshBuffers> m_meshBuffers;

};


}