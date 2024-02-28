#include <stdio.h>

#include <wv/Core/cApplication.h>

#include <cEditorConfig.h>
#include <cGameConfig.h>

#if defined( WV_DEBUG )
#elif defined( WV_RELEASE )
#endif

int main()
{
	wv::cApplication::getInstance().create();

#if defined( WV_DEBUG )
	wv::cApplication::getInstance().run( new cGameConfig() );
#elif defined( WV_RELEASE )
	wv::cApplication::getInstance().run( new cEditorConfig() );
#endif

	wv::cApplication::destroy();
}