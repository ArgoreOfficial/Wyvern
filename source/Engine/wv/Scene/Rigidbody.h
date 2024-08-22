#pragma once

#include <wv/Scene/SceneObject.h>

#include <wv/Physics/Physics.h>
#include <wv/Reflection/Reflection.h>

#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	struct sMeshNode; 

///////////////////////////////////////////////////////////////////////////////////////

	class cRigidbody : public iSceneObject
	{
	public:
		 cRigidbody( const UUID& _uuid, const std::string& _name, sMeshNode* _pMeshNode, iPhysicsBodyDesc* _bodyDesc );
		 cRigidbody( const UUID& _uuid, const std::string& _name, const std::string& _meshPath, iPhysicsBodyDesc* _bodyDesc );
		~cRigidbody();

		static cRigidbody* parseInstance( sParseData& _data );

///////////////////////////////////////////////////////////////////////////////////////

	protected:
		void onLoadImpl() override;
		void onUnloadImpl() override;
		void onCreateImpl() override { };
		void onDestroyImpl() override { };

		virtual void updateImpl( double _deltaTime ) override;
		virtual void drawImpl  ( wv::iDeviceContext* _context, wv::iGraphicsDevice* _device ) override;

		sMeshNode*  m_pMeshNode = nullptr;
		std::string m_meshPath  = "";

		iPhysicsBodyDesc* m_pPhysicsBodyDesc = nullptr;
		hPhysicsBody m_physicsBodyHandle = 0;

	};

}

