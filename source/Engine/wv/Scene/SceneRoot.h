#pragma once

#include "SceneObject.h"

namespace wv
{
	class cSceneRoot : public iSceneObject
	{
	public:

		 cSceneRoot( const std::string& _name );
		~cSceneRoot();

	protected:

		void onLoadImpl   () override { };
		void onUnloadImpl () override { };
		void onCreateImpl () override { };
		void onDestroyImpl() override { };

		void updateImpl( double _deltaTime ) override;
		void drawImpl( iDeviceContext* _context, iGraphicsDevice* _device ) override;
	};
}