#pragma once
#include <Wyvern/Assets/cMaterial.h>
#include <Wyvern/Assets/cMesh.h>
#include <Wyvern/Math/Vector3.h>
#include <Wyvern/Assets/iAsset.h>

namespace wv
{
	class cModel : iAsset
	{
	public:
		 cModel ( std::string _path );
		~cModel( void );

		void load( void ) override;

		void setPosition( cVector3f _position );
		void setRotation( cVector3f _euler );
		void setScale   ( cVector3f _scale );

		void move( cVector3f _value );
		void rotate( cVector3f _euler );

		void render( void );

	private:

		cVector3f m_position{ 0.0f, 0.0f, 0.0f };
		cVector3f m_rotation{ 0.0f, 0.0f, 0.0f };
		cVector3f m_scale   { 1.0f, 1.0f, 1.0f };

		std::vector<cMesh*> m_meshes;
		
		cMaterial m_material;
	};
}