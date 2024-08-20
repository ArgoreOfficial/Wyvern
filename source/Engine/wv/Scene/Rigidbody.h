#pragma once

#include <wv/Scene/SceneObject.h>
#include <wv/Types.h>

#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv { class Mesh; }

///////////////////////////////////////////////////////////////////////////////////////

class cRigidbody : public wv::iSceneObject
{

public:

	 cRigidbody( const uint64_t& _uuid, const std::string& _name, wv::Mesh* _mesh );
	~cRigidbody();

///////////////////////////////////////////////////////////////////////////////////////

protected:

	void onLoadImpl() override;
	void onUnloadImpl() override;
	void onCreateImpl() override { };
	void onDestroyImpl() override { };

	virtual void updateImpl( double _deltaTime ) override;
	virtual void drawImpl  ( wv::iDeviceContext* _context, wv::iGraphicsDevice* _device ) override;

	wv::Mesh* m_mesh = nullptr;

	wv::Handle m_physicsBodyHandle = 0;

};
