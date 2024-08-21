#pragma once

#include <wv/Scene/SceneObject.h>

#include <wv/Physics/PhysicsBodyDescriptor.h>

#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class Mesh; 

///////////////////////////////////////////////////////////////////////////////////////

	class cRigidbody : public iSceneObject
	{
	public:
		 cRigidbody( const UUID& _uuid, const std::string& _name, Mesh* _pMesh, iPhysicsBodyDesc* _bodyDesc );
		~cRigidbody();

		virtual nlohmann::json dataToJson( void ) override;

///////////////////////////////////////////////////////////////////////////////////////

	protected:
		void onLoadImpl() override;
		void onUnloadImpl() override;
		void onCreateImpl() override { };
		void onDestroyImpl() override { };

		virtual void updateImpl( double _deltaTime ) override;
		virtual void drawImpl  ( wv::iDeviceContext* _context, wv::iGraphicsDevice* _device ) override;

		wv::Mesh* m_pMesh = nullptr;
		wv::iPhysicsBodyDesc* m_pPhysicsBodyDesc = nullptr;
		wv::Handle m_physicsBodyHandle = 0;

	};

	template<>
	inline cRigidbody* fromJson<cRigidbody>( nlohmann::json& _json )
	{
		wv::UUID    uuid = _json.value( "uuid", 0 );
		std::string name = _json.value( "name", "" );

		nlohmann::json tfm = _json[ "transform" ];
		std::vector<float> pos = tfm[ "pos" ].get<std::vector<float>>();
		std::vector<float> rot = tfm[ "rot" ].get<std::vector<float>>();
		std::vector<float> scl = tfm[ "scl" ].get<std::vector<float>>();

		Transformf transform;
		transform.setPosition( { pos[ 0 ], pos[ 1 ], pos[ 2 ] } );
		transform.setRotation( { rot[ 0 ], rot[ 1 ], rot[ 2 ] } );
		transform.setScale   ( { scl[ 0 ], scl[ 1 ], scl[ 2 ] } );

		nlohmann::json data = _json[ "data" ];

		ePhysicsKind  kind  = data.value( "kind",  ePhysicsKind::WV_PHYSICS_STATIC );
		ePhysicsShape shape = data.value( "shape", ePhysicsShape::WV_PHYSICS_BOX );

		std::vector<float> halfExtents = data["halfExtents"].get<std::vector<float>>();

		sPhysicsBoxDesc* desc = new sPhysicsBoxDesc();
		desc->kind = kind;
		desc->halfExtent.x = halfExtents[ 0 ];
		desc->halfExtent.y = halfExtents[ 1 ];
		desc->halfExtent.z = halfExtents[ 2 ];
		
		cRigidbody* rb = new cRigidbody( uuid, name, nullptr, desc );
		rb->m_transform = transform;
		return rb;
	}
}

