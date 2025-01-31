#pragma once

#include <wv/Debug/Print.h>
#include <cstring>

namespace wv
{
	namespace Trace
	{
		struct sTrace
		{
			static inline bool printEnabled = 0;
			static inline int depth = 0;

			sTrace( const char* _func )
			{
			#ifdef WV_DEBUG
				if ( sTrace::printEnabled )
				{
					int d = depth * 3;

					char* spacing = new char[ d + 1 ];
					memset( spacing, ' ', d );
					spacing[ d ] = 0;
					
					const unsigned char line[] = { 195, 196, 196, 194, 0 };

					wv::Debug::Print( wv::Debug::WV_PRINT_TRACE, "%s%s%s\n", spacing, line, _func );

					delete[] spacing;
				}

				sTrace::depth++;
			#endif
			}

			~sTrace()
			{
			#ifdef WV_DEBUG
				sTrace::depth--;
				if ( sTrace::depth < 0 )
					sTrace::depth = 0;
			#endif
			}
		};
		
	#ifdef WV_DEBUG
	#define WV_TRACE() wv::Trace::sTrace scopedTrace( __FUNCTION__ )
	#else
	#define WV_TRACE()
	#endif
	}
}