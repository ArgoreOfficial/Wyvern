#pragma once
#include <Wyvern/Assets/iAsset.h>

namespace wv
{
	class cTexture2D : public iAsset
	{
	public:

		 cTexture2D( std::string _path ) : iAsset( _path ) { }
		~cTexture2D();

		void load( void ) override;
		void bind( void );
	private:

		unsigned int m_handle = 0;

	};
}