#pragma once

#include <wv/Scene/SceneObject.h>

#include <wv/Mesh/MeshResource.h>

#include <wv/Physics/Physics.h>
#include <wv/Reflection/Reflection.h>

#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	struct sMeshNode; 
	struct cMeshResource; 

///////////////////////////////////////////////////////////////////////////////////////

	class cRigidbody : public IEntity
	{
	public:
		 cRigidbody( const UUID& _uuid, const std::string& _name, const std::string& _meshPath, iPhysicsBodyDesc* _bodyDesc );
		~cRigidbody();

		static cRigidbody* parseInstance( sParseData& _data );

///////////////////////////////////////////////////////////////////////////////////////

	protected:
		void onLoadImpl() override;
		void onUnloadImpl() override;
		void onCreateImpl() override { };
		void onDestroyImpl() override { };

		virtual void onUpdate( double _deltaTime ) override;
		virtual void drawImpl  ( wv::iDeviceContext* _context, wv::iLowLevelGraphics* _device ) override;

		sMeshInstance m_mesh;
		std::string m_meshPath  = "";

		iPhysicsBodyDesc* m_pPhysicsBodyDesc = nullptr;
		PhysicsBodyID m_physicsBodyHandle = 0;

	};

}

