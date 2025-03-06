#include "camera.h"

#include <wv/engine.h>
#include <wv/device/device_context.h>
#include <wv/graphics/graphics_device.h>
#include <wv/graphics/gpu_buffer.h>

///////////////////////////////////////////////////////////////////////////////////////

wv::ICamera::ICamera( CameraType _type, float _fov, float _near, float _far ) :
	fov   { _fov  },
	m_near{ _near },
	m_far { _far  },
	m_type{ _type }
{
	IGraphicsDevice* pGraphics = Engine::get()->graphics;

	GPUBufferDesc ubDesc;
	ubDesc.name  = "UbCameraData";
	ubDesc.size  = sizeof( UbCameraData );
	ubDesc.type  = WV_BUFFER_TYPE_UNIFORM;
	ubDesc.usage = WV_BUFFER_USAGE_DYNAMIC_DRAW;

	m_uniformBufferID = pGraphics->createGPUBuffer( ubDesc );
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::ICamera::beginRender( IGraphicsDevice* _pGraphicsDevice, FillMode _fillMode )
{
	if ( !_pGraphicsDevice->m_uniformBindingNameMap.count( "UbCameraData" ) )
		return false;

	_pGraphicsDevice->setFillMode( _fillMode );

	UbCameraData instanceData;
	instanceData.projection = getProjectionMatrix();
	instanceData.view       = getViewMatrix();
	instanceData.model      = wv::Matrix4x4f( 1.0f );

	//BufferBindingIndex index = _pGraphicsDevice->m_uniformBindingNameMap.at( "UbCameraData" );
	_pGraphicsDevice->bufferSubData( m_uniformBufferID, &instanceData, sizeof( UbCameraData ), 0 );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Vector3f wv::ICamera::screenToWorld( int _pixelX, int _pixelY, float _depth )
{
	wv::IDeviceContext* ctx = wv::Engine::get()->context;
	float clipX = static_cast<float>( _pixelX ) / static_cast<float>( ctx->getWidth() );
	float clipY = static_cast<float>( _pixelY ) / static_cast<float>( ctx->getHeight() );
	
	clipX = clipX * 2.0f - 1.0f;
	clipY = clipY * 2.0f - 1.0f;

	return screenToWorld( clipX, -clipY, _depth );
}

wv::Vector3f wv::ICamera::screenToWorld( float _clipX, float _clipY, float _depth )
{
	Matrix4x4f viewProj = getViewMatrix() * getProjectionMatrix();
	Matrix4x4f invViewProj = viewProj.inverse();
	
	Vector4f screenspacePoint{ _clipX, _clipY, -_depth, 1.0f };
	Vector4f worldPoint = screenspacePoint * invViewProj;

	return Vector3f{ 
		worldPoint.x / worldPoint.w, 
		worldPoint.y / worldPoint.w, 
		worldPoint.z / worldPoint.w 
	};
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Matrix4x4f wv::ICamera::getProjectionMatrix( void )
{
	switch( m_type )
	{
	case WV_CAMERA_TYPE_PERSPECTIVE:  return getPerspectiveMatrix (); break;
	case WV_CAMERA_TYPE_ORTHOGRAPHIC: return getOrthographicMatrix(); break;
	}

	return Matrix4x4f{ 1.0f };
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Matrix4x4f wv::ICamera::getPerspectiveMatrix( void )
{
	wv::Engine* engine = wv::Engine::get();
	
	float aspect = engine->context->getAspect();
	float sensorH = 36.0f * aspect;
	float sensorW = 36.0f;

	/// TODO: class FocalCamera ?

	//return Math::perspective( engine->context->getAspect(), Math::radians( fov ), m_near, m_far );
	return Math::focalPerspective( sensorW, sensorH, 30.0f, m_near, m_far );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Matrix4x4f wv::ICamera::getOrthographicMatrix( void )
{
	wv::IDeviceContext* ctx = wv::Engine::get()->context;
	float w = (float)ctx->getWidth()  / 2.0f;
	float h = (float)ctx->getHeight() / 2.0f;

	return Math::orthographic( w, h, -1000.0f, 1000.0f );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Matrix4x4f wv::ICamera::getViewMatrix( void )
{
	return m_transform.getMatrix().inverse();
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Vector3f wv::ICamera::getViewDirection()
{
	float yaw   = Math::radians( m_transform.rotation.y - 90.0f );
	float pitch = Math::radians( m_transform.rotation.x );

	Vector3f direction;
	direction.x = std::cos( yaw ) * std::cos( pitch );
	direction.y = std::sin( pitch );
	direction.z = std::sin( yaw ) * std::cos( pitch );
    return direction;
}
