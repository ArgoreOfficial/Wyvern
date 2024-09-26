#pragma once

#include <vector>
#include <string>

#include <wv/Material/MaterialVariable.h>
#include <wv/Math/Matrix.h>
#include <wv/Resource/Resource.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class cProgramPipeline;
	
	class iGraphicsDevice;
	class sMesh;

///////////////////////////////////////////////////////////////////////////////////////

	struct sUbInstanceData
	{
		cMatrix4x4f projection;
		cMatrix4x4f view;
		cMatrix4x4f model;
	};

///////////////////////////////////////////////////////////////////////////////////////

	class cMaterial : public iResource
	{

	public:

		cMaterial( std::string _name, std::string _path ) :
			iResource( _name, _path )
		{ }

		void load( cFileSystem* _pFileSystem, iGraphicsDevice* _pGraphicsDevice ) override;
		void unload( cFileSystem* _pFileSystem, iGraphicsDevice* _pGraphicsDevice ) override;

		void setAsActive( iGraphicsDevice* _device );

		void setMaterialUniforms();
		void setInstanceUniforms( sMesh* _instance );

		cProgramPipeline* getPipeline() { return m_pPipeline; }

///////////////////////////////////////////////////////////////////////////////////////

		std::vector<sMaterialVariable> m_variables;
	protected:

		void setDefaultViewUniforms();
		void setDefaultMeshUniforms( sMesh* _mesh );

		cProgramPipeline* m_pPipeline = nullptr;
		//std::vector<Texture*> m_textures;

		sUbInstanceData m_UbInstanceData;

	};

}
