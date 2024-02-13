#pragma once

#include <wv/Scene/cScene.h>

namespace wv
{

	class cSceneLoader 
	{
	public:
		virtual ~cSceneLoader( void ) { }

		virtual void load( cScene* _scene ) = 0;

		std::string getName() { return m_name; }

	protected:
		cSceneLoader( std::string _name ) :
			m_name{ _name }
		{
		}

		std::string m_name = "nullscene";

	};
}
