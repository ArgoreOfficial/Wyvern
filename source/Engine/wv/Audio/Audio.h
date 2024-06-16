#pragma once

#include <wv/Auxiliary/miniaudio.h>
#include <wv/Debug/Print.h>

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	struct Audio
	{
		ma_sound sound;

///////////////////////////////////////////////////////////////////////////////////////

		inline void play( float _volume = 1.0f, bool _loop = false )
		{
			ma_result res = ma_sound_start( &sound );
			ma_sound_set_volume( &sound, _volume );
			ma_sound_set_looping( &sound, _loop );

			if ( res != MA_SUCCESS )
				Debug::Print( Debug::WV_PRINT_ERROR, "Audio failed to start\n" );
			else
				Debug::Print( Debug::WV_PRINT_DEBUG, "Played sound\n" );

		}

		inline void stop() { ma_sound_stop( &sound ); }

		inline bool isPlaying() { return ma_sound_is_playing( &sound ); }

	};

}