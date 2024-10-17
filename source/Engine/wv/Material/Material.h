#pragma once

#include <vector>
#include <string>

#include <wv/Material/MaterialVariable.h>
#include <wv/Math/Matrix.h>
#include <wv/Resource/Resource.h>
#include <wv/Graphics/Types.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class cShaderResource;
	
	class iLowLevelGraphics;
	class sMesh;

///////////////////////////////////////////////////////////////////////////////////////

	class cMaterial : public iResource
	{

	public:

		cMaterial( std::string _name, std::string _path ) :
			iResource( _name, _path )
		{ }

		void load( cFileSystem* _pFileSystem, iLowLevelGraphics* _pLowLevelGraphics ) override;
		void unload( cFileSystem* _pFileSystem, iLowLevelGraphics* _pLowLevelGraphics ) override;

		void setAsActive( iLowLevelGraphics* _device );

		void setMaterialUniforms();
		void setInstanceUniforms( sMesh* _instance );

		cShaderResource* getShader() { return m_pShader; }

///////////////////////////////////////////////////////////////////////////////////////

		std::vector<sMaterialVariable> m_variables;
	protected:

		void setDefaultMeshUniforms( sMesh* _mesh );

		cShaderResource* m_pShader = nullptr;
		sUbCameraData m_UbCameraData{};

	};

}
