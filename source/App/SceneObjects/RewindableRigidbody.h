#pragma once

#include <wv/Scene/Rigidbody.h>

#include "TimelineRecorder.h"

namespace psq
{
	class cRewindableRigidbody : public wv::cRigidbody
	{
	public:
		 cRewindableRigidbody( const wv::UUID& _uuid, const std::string& _name, const std::string& _meshPath, wv::iPhysicsBodyDesc* _bodyDesc );
		~cRewindableRigidbody() {}

		static cRewindableRigidbody* createInstance() { return nullptr; }
		static cRewindableRigidbody* createInstanceJson( nlohmann::json& _json );

		void setTargetPosition( const wv::cVector3f& _target ) { m_targetPosition = _target; }

	protected:
		virtual void updateImpl( double _deltaTime ) override;

	private:
		cTimelineRecorder m_recorder;
		bool m_physicsUpToDate = true;

		wv::cVector3f m_targetPosition{};

		wv::cVector3f m_velocity{};
		wv::cVector3f m_angularVelocity{};
	};

}
	
REFLECT_CLASS( psq::cRewindableRigidbody );