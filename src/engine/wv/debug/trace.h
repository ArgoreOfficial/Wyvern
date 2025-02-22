#pragma once

#include <wv/debug/log.h>

#include <cstring>

namespace wv
{
	namespace Trace
	{
		struct Trace
		{
			static inline bool printEnabled = 0;
			static inline int depth = 0;

			Trace( const char* _func )
			{
			#ifdef WV_DEBUG
				if ( Trace::printEnabled )
				{
					int d = depth * 3;

					char* spacing = new char[ d + 1 ];
					memset( spacing, ' ', d );
					spacing[ d ] = 0;
					
					const unsigned char line[] = { 195, 196, 196, 194, 0 };

					wv::Debug::Print( wv::Debug::WV_PRINT_TRACE, "%s%s%s\n", spacing, line, _func );

					delete[] spacing;
				}

				Trace::depth++;
			#endif
			}

			~Trace()
			{
			#ifdef WV_DEBUG
				Trace::depth--;
				if ( Trace::depth < 0 )
					Trace::depth = 0;
			#endif
			}
		};
		
	#ifdef WV_DEBUG
	#define WV_TRACE() wv::Trace::Trace scopedTrace( __FUNCTION__ )
	#else
	#define WV_TRACE()
	#endif
	}
}