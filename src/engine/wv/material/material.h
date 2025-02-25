#pragma once

#include <vector>
#include <string>

#include <wv/material/material_variable.h>
#include <wv/math/matrix.h>
#include <wv/resource/resource.h>
#include <wv/graphics/types.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class ShaderResource;
	
	class IGraphicsDevice;
	class Mesh;

///////////////////////////////////////////////////////////////////////////////////////

	class Material : public IResource
	{

	public:

		Material( std::string _name, std::string _path ) :
			IResource( _name, _path )
		{ }

		void load( IFileSystem* _pFileSystem, IGraphicsDevice* _pGraphicsDevice ) override;
		void unload( IFileSystem* _pFileSystem, IGraphicsDevice* _pGraphicsDevice ) override;

		void setAsActive( IGraphicsDevice* _device );

		void setMaterialUniforms();
		void setInstanceUniforms( Mesh* _instance );

		ShaderResource* getShader() { return m_pShader; }

///////////////////////////////////////////////////////////////////////////////////////

		std::vector<MaterialVariable> m_variables;
	protected:

		void setDefaultMeshUniforms( Mesh* _mesh );

		ShaderResource* m_pShader = nullptr;
		UbCameraData m_UbCameraData{};

	};

}
