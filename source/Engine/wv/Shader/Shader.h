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
	
///////////////////////////////////////////////////////////////////////////////////////

	class cShader : public iResource
	{
	public:
		friend class iGraphicsDevice;
		
		cShader( iGraphicsDevice* _pGraphicsDevice, eShaderType _type, const std::string& _name ) :
			iResource{_name, L""},
			m_type{_type},
			m_pGraphicsDevice{_pGraphicsDevice}
		{ }

		void setSource( const std::string& _source ) { m_source = _source; }
		std::string getSource( void ) { return m_source; }

	private:
		iGraphicsDevice* m_pGraphicsDevice;

		eShaderType m_type;
		std::string m_source;
	};


}