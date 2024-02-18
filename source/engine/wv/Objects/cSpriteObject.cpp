#include "cSpriteObject.h"

wv::cSpriteObject::cSpriteObject( std::string _name ) : iSceneObject( _name )
{

}

wv::cSpriteObject* wv::cSpriteObject::create( std::string _path )
{
	m_sprite.create( _path );
	m_sprite.getTransform().scale = { 1.0f, 1.0f, 1.0f };

	m_sprite.getTransform().position.x += 1.7f;
	return this;
}

void wv::cSpriteObject::update( double _delta_time )
{
	m_sprite.getTransform().position.z += _delta_time * 0.07f;
}

void wv::cSpriteObject::render()
{
	m_sprite.render();
}
