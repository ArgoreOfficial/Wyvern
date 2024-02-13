#pragma once

#include <wv/Scene/iSceneObject.h>
#include <wv/Graphics/cSprite.h>

namespace wv
{
	class cSpriteObject : public iSceneObject
	{
	public:
		 cSpriteObject( std::string _name );
		~cSpriteObject() { }

		void create( std::string _path );

		void update( double _delta_time ) override;
		void render() override;

	private:
		wv::cSprite m_sprite;

	};
}