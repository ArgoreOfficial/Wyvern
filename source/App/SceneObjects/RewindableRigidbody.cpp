#include "RewindableRigidbody.h"

#include <wv/Engine/Engine.h>

#include "RewindSettingsWindow.h"

#include <wv/Physics/PhysicsEngine.h>

psq::cRewindableRigidbody::cRewindableRigidbody( const wv::UUID& _uuid, const std::string& _name, const std::string& _meshPath, wv::iPhysicsBodyDesc* _bodyDesc ):
	wv::cRigidbody( _uuid, _name, _meshPath, _bodyDesc )
{
	m_recorder.startRecordingVariable( &m_transform );
	m_recorder.startRecordingVariable( &m_velocity );
	m_recorder.startRecordingVariable( &m_angularVelocity );

	m_recorder.setEnabled( true );
}

// copy pasted from cRigidbody
// required for reflection to work, if only we had virtual statics :((((
psq::cRewindableRigidbody* psq::cRewindableRigidbody::createInstanceJson( nlohmann::json& _json )
{
	wv::UUID    uuid = _json.value( "uuid", wv::cEngine::getUniqueUUID() );
	std::string name = _json.value( "name", "" );

	nlohmann::json tfm = _json[ "transform" ];
	std::vector<float> pos = tfm[ "pos" ].get<std::vector<float>>();
	std::vector<float> rot = tfm[ "rot" ].get<std::vector<float>>();
	std::vector<float> scl = tfm[ "scl" ].get<std::vector<float>>();

	wv::Transformf transform;
	transform.setPosition( { pos[ 0 ], pos[ 1 ], pos[ 2 ] } );
	transform.setRotation( { rot[ 0 ], rot[ 1 ], rot[ 2 ] } );
	transform.setScale( { scl[ 0 ], scl[ 1 ], scl[ 2 ] } );

	nlohmann::json data = _json[ "data" ];

	wv::ePhysicsKind  kind  = data.value( "kind",  wv::ePhysicsKind::WV_PHYSICS_STATIC );
	wv::ePhysicsShape shape = data.value( "shape", wv::ePhysicsShape::WV_PHYSICS_BOX );

	wv::iPhysicsBodyDesc* desc = nullptr;

	switch( shape )
	{
	case wv::WV_PHYSICS_BOX:
	{
		std::vector<float> halfExtents = data[ "halfExtents" ].get<std::vector<float>>();

		wv::sPhysicsBoxDesc* boxDesc = new wv::sPhysicsBoxDesc();
		boxDesc->halfExtent.x = halfExtents[ 0 ];
		boxDesc->halfExtent.y = halfExtents[ 1 ];
		boxDesc->halfExtent.z = halfExtents[ 2 ];
		desc = boxDesc;
	} break;

	case wv::WV_PHYSICS_SPHERE:
	{
		wv::sPhysicsSphereDesc* sphereDesc = new wv::sPhysicsSphereDesc();
		sphereDesc->radius = data.value( "radius", 1.0f );
		desc = sphereDesc;
	} break;
	}

	if( desc )
		desc->kind = kind;

	std::string meshPath = data.value( "path", "" );

	cRewindableRigidbody* rb = new cRewindableRigidbody( uuid, name, meshPath, desc );
	rb->m_transform = transform;
	return rb;
}



void psq::cRewindableRigidbody::updateImpl( double _deltaTime )
{
	wv::cJoltPhysicsEngine* pPhysics = wv::cEngine::get()->m_pPhysicsEngine;

	cRigidbody::updateImpl( _deltaTime );

	m_recorder.setEnabled( !cRewindSettingsWindow::worldIsPaused );
	
	m_recorder.setRewinding ( cRewindSettingsWindow::isRewinding );
	m_recorder.setFrameSpeed( cRewindSettingsWindow::rewindSpeed );

	pPhysics->setBodyActive( m_physicsBodyHandle, !cRewindSettingsWindow::worldIsPaused );

	if( !cRewindSettingsWindow::worldIsPaused )
	{
		if( m_physicsUpToDate )
		{
			m_velocity        = pPhysics->getBodyVelocity       ( m_physicsBodyHandle );
			m_angularVelocity = pPhysics->getBodyAngularVelocity( m_physicsBodyHandle );
		}

		m_recorder.update();
	
		if( pPhysics->isBodyActive( m_physicsBodyHandle ) )
		{
			pPhysics->setBodyTransform      ( m_physicsBodyHandle, m_transform );
			pPhysics->setBodyVelocity       ( m_physicsBodyHandle, m_velocity );
			pPhysics->setBodyAngularVelocity( m_physicsBodyHandle, m_angularVelocity );
			m_physicsUpToDate = true;
		}
	}
	else
	{
		m_recorder.setFrame( cRewindSettingsWindow::currentFrame );
		m_physicsUpToDate = false;
	}
}
