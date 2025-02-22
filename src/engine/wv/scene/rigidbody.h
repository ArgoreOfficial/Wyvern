#pragma once

#include <wv/scene/entity/entity.h>

#include <wv/mesh/mesh_resource.h>

#include <wv/physics/physics.h>
#include <wv/reflection/reflection.h>

#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	struct sMeshNode; 
	struct cMeshResource; 

///////////////////////////////////////////////////////////////////////////////////////

	class cRigidbody : public Entity
	{
	public:
		 cRigidbody( const UUID& _uuid, const std::string& _name );
		 cRigidbody( const UUID& _uuid, const std::string& _name, const std::string& _meshPath, iPhysicsBodyDesc* _bodyDesc );
		~cRigidbody();

		static cRigidbody* parseInstance( sParseData& _data );

		std::string m_meshPath;
		iPhysicsBodyDesc* m_bodyDesc = nullptr;
	};

}

