#pragma once

#include <Wyvern/Assets/iResource.h>

namespace wv
{
	class cTexture2D : public iResource
	{
	public:

		 cTexture2D( void ) : iResource() { }
		~cTexture2D( void ) { }

		void load  ( std::string _path ) override;
		void create( void ) override;
		void bind( void );
	
	private:

		unsigned int m_handle = 0;
		unsigned char* m_data = 0;
		int m_width  = 0;
		int m_height = 0;
		int m_nrChannels = 0;

	};
}