#pragma once

#include <miniaudio.h>

namespace wv
{
	struct Audio
	{
		ma_sound sound;

		inline void play()
		{
			ma_sound_start( &sound );
		}
	};
}