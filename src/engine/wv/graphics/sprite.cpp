#include "sprite.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <fstream>
#include <sstream>

#include <wv/camera/camera.h>

#include <algorithm>

///////////////////////////////////////////////////////////////////////////////////////

std::string loadTextFile( const std::string& _path )
{
	std::ifstream t( _path );
	std::stringstream buffer;
	buffer << t.rdbuf();

	return buffer.str();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Sprite::create( OpenGLRenderer& _renderer, const std::string& _texture_path, const std::string& _shader_vert_path, const std::string& _shader_frag_path )
{
	
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Sprite::destroy( OpenGLRenderer& _renderer )
{
	_renderer.destroyVertexBuffer( vertex_buffer );
	_renderer.destroyPipeline( shader_pipeline );
	_renderer.destroyTexture( texture );
}

void wv::Sprite::update()
{
	updateCollider();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Sprite::draw( OpenGLRenderer& _renderer )
{
	
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Sprite::updateCollider()
{
	collider_box.center.x = transform.position.x;
	collider_box.center.y = transform.position.y;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Vector3f wv::Sprite::getTrueSize() const
{
	wv::Vector3f scale = transform.scale;

	return wv::Vector3f(
		{
			scale.x * sprite_size.x,
			scale.y * sprite_size.y,
			1.0f,
		} );
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::Sprite::contains( const wv::AABB2Df& _other )
{
	updateCollider();
	return collider_box.contains( _other );
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::Sprite::contains( const Sprite& _other )
{
	updateCollider();
	return collider_box.contains( _other.collider_box );
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::Sprite::contains( const wv::Vector2f& _point )
{
	updateCollider();
	return collider_box.contains( _point );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::ResourceID wv::SpriteRenderer::loadTexture( const std::string& _texture_path )
{
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load( false );

	unsigned char* data = stbi_load( _texture_path.c_str(), &width, &height, &nrChannels, 0 );
	if ( data == nullptr )
		data = stbi_load( "debug.png", &width, &height, &nrChannels, 0 );

	ResourceID texture = m_renderer->createTexture( data, width, height, nrChannels, wv::WV_TEXTURE_FORMAT_BYTE, false, wv::WV_TEXTURE_FILTER_NEAREST );
	stbi_image_free( data );

	return texture;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Sprite* wv::SpriteRenderer::createSprite( const std::string& _texture_path, const std::string& _vs_path, const std::string& _fs_path )
{
	// Texture
	ResourceID texture = loadTexture( _texture_path );
	Vector2i tex_size = m_renderer->getTextureSize( texture );

	// sprite
	wv::Sprite* sprite = createSpriteFromTexture( texture, { 0, 0 }, tex_size, _vs_path, _fs_path );
	
	return sprite;
}

wv::Sprite* wv::SpriteRenderer::createSpriteFromTexture( ResourceID _texture, const wv::Vector2i& _sprite_offset, const wv::Vector2i& _sprite_size, const std::string& _vs_path, const std::string& _fs_path )
{
	wv::Sprite* sprite = new wv::Sprite();
	sprite->texture = _texture;

	{
		// Shader
		std::string shader_src_vert = loadTextFile( _vs_path );
		std::string shader_src_frag = loadTextFile( _fs_path );

		sprite->shader_pipeline = m_renderer->createPipeline( shader_src_vert.c_str(), shader_src_frag.c_str() );
		m_renderer->bindPipeline( sprite->shader_pipeline );

		wv::Vector2i tex_size = m_renderer->getTextureSize( _texture );
		
		wv::Vector2f uv_tl{ 
			(float)_sprite_offset.x / (float)tex_size.x,
			(float)_sprite_offset.y / (float)tex_size.y
		};

		wv::Vector2f uv_br{ 
			(float)( _sprite_offset.x + _sprite_size.x ) / (float)tex_size.x,
			(float)( _sprite_offset.y + _sprite_size.y ) / (float)tex_size.y
		};
		
		sprite->texture_size = wv::Vector2f{ 
			(float)( _sprite_offset.x + _sprite_size.x ), 
			(float)( _sprite_offset.y + _sprite_size.y ) 
		};

		// Vertices
		Vertex screen_triangle[] = {
			Vertex( -0.5f,  0.5f, uv_tl.x, uv_tl.y ), // top left
			Vertex(  0.5f,  0.5f, uv_br.x, uv_tl.y ), // top right
			Vertex(  0.5f, -0.5f, uv_br.x, uv_br.y ), // bottom right

			Vertex( -0.5f,  0.5f, uv_tl.x, uv_tl.y ), // top left
			Vertex(  0.5f, -0.5f, uv_br.x, uv_br.y ), // bottom right
			Vertex( -0.5f, -0.5f, uv_tl.x, uv_br.y ), // bottom left
		};

		sprite->vertex_buffer = m_renderer->createVertexBuffer( screen_triangle, sizeof( screen_triangle ) );

	}

	m_sprites.insert( sprite );
	
	return sprite;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::SpriteRenderer::destroySprite( Sprite* _sprite )
{
	if( m_sprites.contains( _sprite ) )
		m_sprites.erase( _sprite );

	_sprite->destroy( *m_renderer );
	delete _sprite;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::SpriteRenderer::update()
{
	for ( auto s : m_sprites )
		s->update();
	
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::SpriteRenderer::drawSprites( wv::ICamera* _camera )
{
	m_drawlist.clear();

	for ( auto s : m_sprites )
		if( s->is_visible )
			m_drawlist.push_back( s );
	
	for ( auto s : m_drawlist )
	{
		m_renderer->bindPipeline( s->shader_pipeline );

		wv::Matrix4x4f view_proj = _camera->getViewMatrix() * _camera->getProjectionMatrix();

		wv::Vector3f scale = s->transform.scale;
		s->transform.setScale( s->getTrueSize() );

		s->transform.update( nullptr );
		m_renderer->setVSUniformMatrix4x4( s->shader_pipeline, 1, view_proj ); // view proj
		m_renderer->setVSUniformMatrix4x4( s->shader_pipeline, 2, s->transform.getMatrix() ); // model
		m_renderer->setVSUniformVector2f( s->shader_pipeline, 3, s->uv_offset );

		m_renderer->bindTexture( s->texture, 0 );
		m_renderer->bindVertexBuffer( s->vertex_buffer );

		m_renderer->draw( 0, 6 );

		s->transform.setScale( scale );
	}
		
}

