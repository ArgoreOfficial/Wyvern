#pragma once

#ifdef WV_PLATFORM_WINDOWS
#define WV_SUPPORT_MINIAUDIO
#endif

#ifdef WV_SUPPORT_MINIAUDIO
#include <wv/Auxiliary/miniaudio.h>
#endif

#include <wv/Debug/Print.h>

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	struct Audio
	{
	#ifdef WV_SUPPORT_MINIAUDIO
		ma_sound sound;
	#endif

///////////////////////////////////////////////////////////////////////////////////////

		inline void play( float _volume = 1.0f, bool _loop = false )
		{
		#ifdef WV_SUPPORT_MINIAUDIO

			ma_result res = ma_sound_start( &sound );
			ma_sound_set_volume( &sound, _volume );
			ma_sound_set_looping( &sound, _loop );

			if ( res != MA_SUCCESS )
				Debug::Print( Debug::WV_PRINT_ERROR, "Audio failed to start\n" );
			else
				Debug::Print( Debug::WV_PRINT_DEBUG, "Played sound\n" );
		#else
			
			Debug::Print( Debug::WV_PRINT_DEBUG, "Played sound\n" );
		#endif
		}

		inline void stop() 
		{
		#ifdef WV_SUPPORT_MINIAUDIO
			ma_sound_stop( &sound ); 
		#endif
		}

		inline bool isPlaying() 
		{
		#ifdef WV_SUPPORT_MINIAUDIO
			return ma_sound_is_playing( &sound ); 
		#else
			return false;
		#endif
		}

	};

}