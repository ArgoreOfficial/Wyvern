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

	struct MeshNode; 
	struct MeshResource; 

///////////////////////////////////////////////////////////////////////////////////////

	class Rigidbody : public Entity
	{
	public:
		 Rigidbody( const UUID& _uuid, const std::string& _name );
		 Rigidbody( const UUID& _uuid, const std::string& _name, const std::string& _meshPath, IPhysicsBodyDesc* _bodyDesc );
		~Rigidbody();

		static Rigidbody* parseInstance( ParseData& _data );

		std::string m_meshPath;
		IPhysicsBodyDesc* m_bodyDesc = nullptr;
	};

}

