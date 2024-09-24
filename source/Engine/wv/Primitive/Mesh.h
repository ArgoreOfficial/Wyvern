#pragma once

#include <wv/Math/Transform.h>
#include <wv/Math/Triangle.h>
#include <wv/Primitive/Primitive.h>
#include <wv/Types.h>

#include <string>
#include <vector>
#include <queue>

#include <wv/Resource/Resource.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	struct sMeshNode
	{
		std::string name;
		Transformf transform;
		std::vector<sMesh*> meshes;
		std::vector<sMeshNode*> children;
	};

///////////////////////////////////////////////////////////////////////////////////////

	class cMeshResource;

	struct sMeshInstance
	{
		void draw();

		// removes this instance, mesh may stay loaded
		void destroy(); 

		Transformf transform;
		cMeshResource* pResource;
	};

///////////////////////////////////////////////////////////////////////////////////////

	class cMeshResource : public wv::iResource
	{
	public:
		cMeshResource( const std::string& _name, const std::string& _path ) :
			iResource( _name, _path )
		{ }

		void load  ( cFileSystem* _pFileSystem, iGraphicsDevice* _pGraphicsDevice ) override;
		void unload( cFileSystem* _pFileSystem, iGraphicsDevice* _pGraphicsDevice ) override;

		sMeshInstance createInstance();
		void destroyInstance( sMeshInstance& _instance );

		std::vector<Transformf>& getDrawQueue() { return m_drawQueue; }
		
		void addToDrawQueue( sMeshInstance& _instance );

		void drawInstances( iGraphicsDevice* _pGraphicsDevice );

///////////////////////////////////////////////////////////////////////////////////////

		bool m_useGPUInstancing = false;

	private:
		void drawNode( iGraphicsDevice* _pGraphicsDevice, sMeshNode* _node );

		sMeshNode* m_pMeshNode = nullptr;


		std::vector<Transformf> m_drawQueue; /// sMeshInstanceData ?
		std::vector<cMatrix4x4f> m_matrices;
	};


}