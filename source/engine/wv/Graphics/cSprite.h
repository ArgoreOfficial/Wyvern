#pragma once

#include <wv/Graphics/cMaterial.h>

#include <wv/Graphics/cMesh.h>
#include <wv/Math/Transform.h>

#include <string>

namespace wv
{
	class cSprite
	{
	public:

		 cSprite( void );
		~cSprite( void );

		/* TODO: abstract further? */
		void create( const std::string& _material );

		wv::cTransformf& getTransform( void ) { return m_transform; }

		void render( void );

	private:

		wv::cMesh*      m_quad;
		wv::cMaterial*  m_material;
		wv::cTransformf m_transform;

		float m_aspect = 1.0f;

	};
}