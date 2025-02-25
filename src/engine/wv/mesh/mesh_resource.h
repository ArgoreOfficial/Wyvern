#pragma once

#include <wv/math/transform.h>
#include <wv/math/Triangle.h>
#include <wv/graphics/mesh.h>
#include <wv/types.h>

#include <string>
#include <vector>
#include <queue>
#include <mutex>

#include <stdint.h>

#include <wv/resource/resource.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class MeshResource;

	struct MeshInstance
	{
		void draw();

		// removes this instance, mesh may stay loaded
		void destroy(); 

		Transformf transform{};
		MeshResource* pResource = nullptr;
	};

///////////////////////////////////////////////////////////////////////////////////////

	class MeshResource : public wv::IResource
	{
	public:
		MeshResource( const std::string& _name, const std::string& _path ) :
			IResource( _name, _path )
		{ }

		void load  ( FileSystem* _pFileSystem, IGraphicsDevice* _pGraphicsDevice ) override;
		void unload( FileSystem* _pFileSystem, IGraphicsDevice* _pGraphicsDevice ) override;

		void makeInstance( MeshInstance* _instance );
		void removeInstance( MeshInstance* _instance );

		MeshNode*                   getMeshNode ( void ) { return m_pMeshNode; }
		std::vector<MeshInstance*>& getInstances( void ) { return m_instances; }

///////////////////////////////////////////////////////////////////////////////////////

		bool m_useGPUInstancing = false;

	private:
		std::mutex m_mutex{};

		MeshNode* m_pMeshNode = nullptr;
		std::vector<MeshInstance*> m_instances;
	};


}