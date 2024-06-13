#include "StarDestroyer.h"

#include <wv/Application/Application.h>
#include <wv/Device/DeviceContext.h>
#include <wv/Device/AudioDevice.h>
#include <wv/Primitive/Mesh.h>
#include <wv/Camera/ICamera.h>
#include <wv/Math/Math.h>

StarDestroyer::StarDestroyer( wv::Mesh* _mesh ) :
	Ship{ _mesh }
{
	m_maxSpeed = 200.0f;
	m_throttle = 1.0f;
}

StarDestroyer::~StarDestroyer()
{
	wv::Application* app = wv::Application::get();

	m_exitHyperspaceSound->stop();

	app->audio->unloadAudio( m_exitHyperspaceSound );
}

void StarDestroyer::onCreate()
{
	wv::Application* app = wv::Application::get();

	m_exitHyperspaceSound = app->audio->loadAudio3D( "isd_exit_hyperspace.flac" );
	
}

void StarDestroyer::update( double _deltaTime )
{
	wv::Application* app = wv::Application::get();

	if ( !m_arrived && m_arrivedTimer > 0.0f )
	{
		m_arrivedTimer -= _deltaTime;
		if ( m_arrivedTimer < 1.4f && !m_playedHyperspaceSound )
		{
			m_exitHyperspaceSound->play( 140.0f );
			m_playedHyperspaceSound = true;
		}
		
		if ( m_arrivedTimer <= 0.0f )
			m_arrived = true;
	}

	if ( m_exitHyperspaceSound->isPlaying() )
		ma_sound_set_position( &m_exitHyperspaceSound->sound, m_transform.position.x, m_transform.position.y, m_transform.position.z );

	if ( !m_arrived )
		return;

	if ( m_warpTimer < 1.0f )
	{
		m_warpTimer += _deltaTime * 40.0f;
		m_warpTimer = wv::Math::clamp( m_warpTimer, 0.0f, 1.0f );

		m_throttle = wv::Math::lerp( 1.0f, 0.01f, m_warpTimer );
		float warpScale = wv::Math::lerp( 1.3f, 1.04f, m_warpTimer );

		m_transform.setScale( { 1.0f, 1.0f, warpScale } );
	}

	Ship::update( _deltaTime );
}

void StarDestroyer::draw( wv::GraphicsDevice* _device )
{
	if ( m_arrived )
		Ship::draw( _device );
}

void StarDestroyer::exitHyperspace( float _timer )
{
	if ( _timer == 0.0f )
		m_arrived = true;
	else
		m_arrivedTimer = _timer;
}
