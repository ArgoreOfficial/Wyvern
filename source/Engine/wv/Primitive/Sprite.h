#pragma once

#include <wv/Math/Transform.h>

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class Mesh;
	class Material;
	class iGraphicsDevice;

///////////////////////////////////////////////////////////////////////////////////////

	namespace Internal
	{
		static Mesh* S_SPRITE_QUAD = nullptr;
	}

///////////////////////////////////////////////////////////////////////////////////////

	struct SpriteDesc
	{
		Mesh* mesh = nullptr;
		Material* material = nullptr;
		Vector3f position{};
		Vector3f size{ 64.0f, 64.0f, 1.0f };
	};

///////////////////////////////////////////////////////////////////////////////////////

	class Sprite
	{

	public:

		~Sprite();

		static Sprite* create( SpriteDesc* _desc );

		Transformf& getTransform() { return m_transform; }
		void draw( iGraphicsDevice* _device );
		Material* getMaterial() { return m_material; }

///////////////////////////////////////////////////////////////////////////////////////

	private:

		Sprite() { };

		static void createQuad();

		Material* m_material = nullptr;
		Transformf m_transform;
		Mesh* m_mesh = nullptr;

	};

}
