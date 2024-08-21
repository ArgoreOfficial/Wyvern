#pragma once

#include <wv/Scene/SceneObject.h>

#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class Mesh; 
	class iPhysicsBodyDesc; 

///////////////////////////////////////////////////////////////////////////////////////

	class cRigidbody : public iSceneObject
	{
	public:
		 cRigidbody( const UUID& _uuid, const std::string& _name, Mesh* _pMesh, iPhysicsBodyDesc* _bodyDesc );
		~cRigidbody();

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

}

