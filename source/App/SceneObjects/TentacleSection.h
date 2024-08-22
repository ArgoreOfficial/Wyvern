#pragma once

#include <wv/Scene/SceneObject.h>

#include <wv/Reflection/Reflection.h>

#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////

namespace wv { class Mesh; }
namespace wv { class cMaterial; }

///////////////////////////////////////////////////////////////////////////////////////

class cTentacleSectionObject : public wv::iSceneObject
{

public:

	 cTentacleSectionObject( const uint64_t& _uuid, const std::string& _name, float _segmentLength );
	~cTentacleSectionObject();

///////////////////////////////////////////////////////////////////////////////////////

	static cTentacleSectionObject* createInstance( void ) { return nullptr; }
	static cTentacleSectionObject* createInstanceJson( nlohmann::json& _json );

///////////////////////////////////////////////////////////////////////////////////////

protected:

	void onLoadImpl() override;
	void onUnloadImpl() override;
	void onCreateImpl() override { };
	void onDestroyImpl() override { };

	virtual void updateImpl( double _deltaTime ) override;
	virtual void drawImpl  ( wv::iDeviceContext* _context, wv::iGraphicsDevice* _device ) override;

	wv::Mesh* m_mesh;
	float m_segmentLength = 1.0f;

};

REFLECT_CLASS( cTentacleSectionObject );