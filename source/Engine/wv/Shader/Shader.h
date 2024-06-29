#pragma once

#include <wv/Resource/Resource.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	enum eShaderType
	{
		WV_SHADER_TYPE_VERTEX,
		WV_SHADER_TYPE_FRAGMENT
	};

///////////////////////////////////////////////////////////////////////////////////////

	class iGraphicsDevice;
	class cFileSystem;

///////////////////////////////////////////////////////////////////////////////////////

	class cShader : public iResource
	{
	public:
		cShader( eShaderType _type, const std::string& _name, const std::wstring& _path = L"" ) :
			iResource{_name, _path},
			m_type{_type}
		{ }
		~cShader();

		void load  ( cFileSystem* _pFileSystem ) override;
		void unload( cFileSystem* _pFileSystem ) override;

		void create( iGraphicsDevice* _pGraphicsDevice ) override;
		void destroy( iGraphicsDevice* _pGraphicsDevice ) override;

		void setSource( const std::string& _source ) { m_source = _source; }
		std::string getSource( void ) { return m_source; }

	private:
		eShaderType m_type;
		std::string m_source;
	};


}