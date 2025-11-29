#pragma once

#include <wv/types.h>
#include <wv/graphics/renderer.h>
#include <wv/math/aabb.h>

#include <wv/math/transform.h>
#include <wv/math/vector2.h>

#include <unordered_set>

namespace wv {

class ICamera;

struct Sprite
{
	void create( OpenGLRenderer& _renderer, const std::string& _texture_path, const std::string& _vs_path, const std::string& _fs_path );
	void destroy( OpenGLRenderer& _renderer );
	void update();
	void draw( OpenGLRenderer& _renderer );

	void updateCollider();

	wv::Vector3f getTrueSize() const;

	bool contains( const wv::AABB2Df& _other );
	bool contains( const Sprite& _other );
	bool contains( const wv::Vector2f& _point );

	wv::Transformf transform{};
	wv::Vector2f texture_size{ 1.0f, 1.0f };
	wv::Vector3f sprite_size{ 16.0f, 16.0f, 1.0f };
	wv::Vector2f uv_offset{ 0.0f, 0.0f };

	wv::AABB2Df collider_box{ };

	ResourceID shader_pipeline = 0;
	ResourceID vertex_buffer = 0;
	ResourceID texture = 0;

	bool is_visible = true;
};

class SpriteRenderer
{
public:

	SpriteRenderer( OpenGLRenderer* _renderer ) : 
		m_renderer { _renderer } 
	{ }

	~SpriteRenderer() { }

	ResourceID loadTexture( const std::string& _texture_path );

	Sprite* createSprite( const std::string& _texture_path, const std::string& _vs_path = "sprite_vs.glsl", const std::string& _fs_path = "sprite_fs.glsl" );
	
	Sprite* createSpriteFromTexture( 
		ResourceID _texture,
		const wv::Vector2i& _sprite_offset,
		const wv::Vector2i& _sprite_size,
		const std::string& _vs_path = "sprite_vs.glsl", 
		const std::string& _fs_path = "sprite_fs.glsl" );
	
	void destroySprite( Sprite* _sprite );

	void update();
	void drawSprites( wv::ICamera* _camera );

private:
	OpenGLRenderer* m_renderer;
	std::unordered_set<Sprite*> m_sprites;
	std::vector<Sprite*> m_drawlist;
};

}