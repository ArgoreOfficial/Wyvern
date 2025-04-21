#include "iupdatable.h"

#include <wv/app_state.h>
#include <wv/scene/scene.h>

///////////////////////////////////////////////////////////////////////////////////////

wv::IUpdatable::~IUpdatable()
{
	unregisterUpdatable();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::IUpdatable::registerUpdatable( wv::Scene* _pScene )
{
	wv::Scene* pScene = _pScene;

	if ( !pScene )
	{
		if ( GetAppState() == nullptr )
		{
			WV_LOG_ERROR( "GetAppState() returned NULL\n" );
			return;
		}

		pScene = GetAppState()->getCurrentScene();
	}

	if ( !pScene )
	{
		WV_LOG_ERROR( "No current scene\n" );
		return;
	}

	pScene->registerUpdatable( this );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::IUpdatable::unregisterUpdatable()
{
	if ( !m_pRegisteredScene )
	{
		WV_LOG_WARNING( "Cannot unregisted IUpdatable that is not registered\n" );
		return;
	}

	m_pRegisteredScene->unregisterUpdatable( this );
}
