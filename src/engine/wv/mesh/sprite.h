#pragma once

#include <wv/graphics/types.h>
#include <wv/math/transform.h>

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class Material;
	class IGraphicsDevice;

///////////////////////////////////////////////////////////////////////////////////////

	namespace Internal
	{
		static MeshID S_SPRITE_QUAD{};
	}

///////////////////////////////////////////////////////////////////////////////////////

	struct SpriteDesc
	{
		MeshID mesh{};
		Material* material = nullptr;
		Vector3f position{};
		Vector3f size{ 64.0f, 64.0f, 1.0f };
	};

///////////////////////////////////////////////////////////////////////////////////////

	class Sprite
	{

	public:

		Sprite() { };
		~Sprite();

		static Sprite* create( SpriteDesc* _desc );

		Transformf& getTransform() { return m_transform; }
		void draw( IGraphicsDevice* _device );
		Material* getMaterial() { return m_material; }

///////////////////////////////////////////////////////////////////////////////////////

	private:


		static void createQuad();

		Material* m_material = nullptr;
		Transformf m_transform;
		MeshID m_mesh{};

	};

}
