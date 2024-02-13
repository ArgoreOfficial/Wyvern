#include "cSpriteObject.h"

wv::cSpriteObject::cSpriteObject( std::string _name ) : iSceneObject( _name )
{

}

void wv::cSpriteObject::create( std::string _path )
{
	m_sprite.create( _path );
	m_sprite.getTransform().scale = { 1.0f, 1.0f, 1.0f };
}

void wv::cSpriteObject::update( double _delta_time )
{
	
}

void wv::cSpriteObject::render()
{
	m_sprite.render();
}
