#pragma once

#include <wv/Scene/SceneObject.h>

#include "TimelineRecorder.h"

namespace psq
{
	class cFollowPath : public wv::iSceneObject
	{
	public:
		 cFollowPath( const wv::UUID& _uuid, const std::string& _name ) : 
			 wv::iSceneObject( _uuid, _name )
		 { }
		~cFollowPath() { }

		static cFollowPath* createInstance() { return new cFollowPath( wv::cEngine::getUniqueUUID(), "FollowPath" ); }
		static cFollowPath* createInstanceJson( nlohmann::json& _json ) 
		{ 
			wv::UUID    uuid = _json.value( "uuid", wv::cEngine::getUniqueUUID() );
			std::string name = _json.value( "name", "" );

			return new cFollowPath( uuid, name );
		};

///////////////////////////////////////////////////////////////////////////////////////

	protected:
		void onLoadImpl   ( void ) override { }
		void onUnloadImpl ( void ) override { }
		void onCreateImpl ( void ) override { }
		void onDestroyImpl( void ) override { }

		virtual void updateImpl( double _deltaTime )                                          override { }
		virtual void drawImpl  ( wv::iDeviceContext* _context, wv::iGraphicsDevice* _device ) override { }
	};

}

REFLECT_CLASS( psq::cFollowPath );