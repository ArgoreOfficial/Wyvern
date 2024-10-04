#pragma once

#include <wv/Math/Transform.h>
#include <wv/Math/Triangle.h>
#include <wv/Mesh/Mesh.h>
#include <wv/Types.h>

#include <string>
#include <vector>
#include <queue>

#include <stdint.h>

#include <wv/Resource/Resource.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

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

	struct sMeshInstanceData
	{
		cMatrix4x4f model;                              // 64 bytes
		uint64_t texturesHandles[ 4 ] = { 0, 0, 0, 0 }; // 32 bytes
		int32_t hasAlpha = 0;

		uint8_t padding0[ 12 ] = { 0 };
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
		void drawNode( iGraphicsDevice* _pGraphicsDevice, sMeshNode* _node, bool _recalcMatrices );

		sMeshNode* m_pMeshNode = nullptr;
		std::vector<Transformf> m_drawQueue;
	};


}