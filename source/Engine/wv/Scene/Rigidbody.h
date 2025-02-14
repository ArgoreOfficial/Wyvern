#pragma once

#include <wv/Scene/Entity/Entity.h>

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

