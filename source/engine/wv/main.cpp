#include <stdio.h>

#include <wv/Core/cApplication.h>

#if defined( WV_DEBUG )
#include <cEditorConfig.h>
#elif defined( WV_RELEASE )
#include <cGameConfig.h>
#endif

int main()
{
	wv::cApplication::getInstance().create();

#if defined( WV_DEBUG )
	wv::cApplication::getInstance().run( new cEditorConfig() );
#elif defined( WV_RELEASE )
	wv::cApplication::getInstance().run( new cGameConfig() );
#endif

	wv::cApplication::destroy();
}