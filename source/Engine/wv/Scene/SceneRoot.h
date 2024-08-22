#pragma once

#include "SceneObject.h"

namespace wv
{
	class cSceneRoot : public iSceneObject
	{
	public:
		cSceneRoot( const std::string& _name, const std::string& _sourcePath = "" ) :
			iSceneObject( 0, _name ),
			m_sourcePath{ _sourcePath }
		{ }

		~cSceneRoot() { }

		std::string getSourcePath() { return m_sourcePath; }

	protected:

		void onLoadImpl   () override { };
		void onUnloadImpl () override { };
		void onCreateImpl () override { };
		void onDestroyImpl() override { };

		void updateImpl( double _deltaTime )                                  override { };
		void drawImpl  ( iDeviceContext* _context, iGraphicsDevice* _device ) override { };

		std::string m_sourcePath = "";
	};
}