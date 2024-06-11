#pragma once

#include <wv/Auxiliary/miniaudio.h>
#include <wv/Debug/Print.h>

namespace wv
{
	struct Audio
	{
		ma_sound sound;

		inline void play()
		{
			ma_result res = ma_sound_start( &sound );
			if ( res != MA_SUCCESS )
				Debug::Print( Debug::WV_PRINT_ERROR, "Audio failed to start\n" );
			else
				Debug::Print( Debug::WV_PRINT_INFO, "Played sound\n" );

		}

		inline bool isPlaying() { return ma_sound_is_playing( &sound ); }
	};
}