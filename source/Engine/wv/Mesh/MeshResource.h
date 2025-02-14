#pragma once

#include <wv/Math/Transform.h>
#include <wv/Math/Triangle.h>
#include <wv/Graphics/Mesh.h>
#include <wv/Types.h>

#include <string>
#include <vector>
#include <queue>
#include <mutex>

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

		Transformf transform{};
		cMeshResource* pResource = nullptr;
	};

///////////////////////////////////////////////////////////////////////////////////////

	class cMeshResource : public wv::iResource
	{
	public:
		cMeshResource( const std::string& _name, const std::string& _path ) :
			iResource( _name, _path )
		{ }

		void load  ( cFileSystem* _pFileSystem, iLowLevelGraphics* _pLowLevelGraphics ) override;
		void unload( cFileSystem* _pFileSystem, iLowLevelGraphics* _pLowLevelGraphics ) override;

		sMeshInstance createInstance();
		void destroyInstance( sMeshInstance& _instance );

		std::vector<Transformf>& getDrawQueue() { return m_drawQueue; }
		
		void addToDrawQueue( sMeshInstance& _instance );

		void drawInstances( iLowLevelGraphics* _pLowLevelGraphics );

///////////////////////////////////////////////////////////////////////////////////////

		bool m_useGPUInstancing = false;

	private:
		void drawNode( iLowLevelGraphics* _pLowLevelGraphics, sMeshNode* _node );

		std::mutex m_mutex{};
		sMeshNode* m_pMeshNode = nullptr;
		std::vector<Transformf> m_drawQueue;
	};


}