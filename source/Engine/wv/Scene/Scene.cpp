#include "Scene.h"

#include <wv/Memory/Memory.h>

///////////////////////////////////////////////////////////////////////////////////////

void wv::Scene::addChild( iSceneObject* _node, bool _triggerLoadAndCreate )
{
	if ( !_node )
		return;

	for ( size_t i = 0; i < m_sceneObjects.size(); i++ )
		if ( m_sceneObjects[ i ] == _node ) return; // node already has child
	
	m_sceneObjects.push_back( _node );

	if ( _triggerLoadAndCreate )
	{
		_node->onCreateImpl();
		_node->onLoadImpl();
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Scene::removeChild( iSceneObject* _node )
{
	if ( !_node )
		return;

	for ( size_t i = 0; i < m_sceneObjects.size(); i++ )
	{
		if ( m_sceneObjects[ i ] != _node )
			continue;

		m_sceneObjects.erase( m_sceneObjects.begin() + i );
		return;
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Scene::onLoad()
{
	Job::JobFunction_t fptr = []( const Job _job, void* _pData )
		{
			JobData* data = (JobData*)_pData;
			wv::iSceneObject* obj = data->pObject;
			if ( !obj->m_loaded )
			{
				obj->onLoadImpl();
				obj->m_loaded = true;
			}
		};

	_runJobs<JobData>( "onLoad", fptr, false );
}

void wv::Scene::onUnload()
{
	Job::JobFunction_t fptr = []( const Job _job, void* _pData )
		{
			JobData* data = (JobData*)_pData;
			wv::iSceneObject* obj = data->pObject;
			if ( obj->m_loaded )
			{
				obj->onUnloadImpl();
				obj->m_loaded = false;
			}
		};

	_runJobs<JobData>( "onUnload", fptr, false );

	for ( size_t i = 0; i < m_sceneObjects.size(); i++ )
		WV_FREE( m_sceneObjects[ i ] );
}

void wv::Scene::onCreate()
{
	Job::JobFunction_t fptr = []( const Job _job, void* _pData )
		{
			JobData* data = (JobData*)_pData;
			wv::iSceneObject* obj = data->pObject;
			if ( !obj->m_created )
			{
				obj->onCreateImpl();
				obj->m_created = true;
			}
		};

	_runJobs<JobData>( "onCreate", fptr, false );
}

void wv::Scene::onDestroy()
{
	Job::JobFunction_t fptr = []( const Job _job, void* _pData )
		{
			JobData* data = (JobData*)_pData;
			wv::iSceneObject* obj = data->pObject;
			if ( obj->m_created )
			{
				obj->onDestroyImpl();
				obj->m_created = false;
			}
		};

	_runJobs<JobData>( "onDestroy", fptr, false );
	
}

void wv::Scene::onUpdate( double _deltaTime )
{
	Job::JobFunction_t fptr = []( const Job _job, void* _pData )
		{
			UpdateData* updateData = (UpdateData*)_pData;
			wv::iSceneObject* obj = updateData->pObject;

			if ( obj->m_loaded && obj->m_created )
				obj->onUpdate( updateData->deltaTime );
		};

	_runJobs<UpdateData>( "onUpdate", fptr, _deltaTime );

	for ( size_t i = 0; i < m_sceneObjects.size(); i++ )
	{
		if ( m_sceneObjects[ i ]->m_transform.pParent != nullptr )
			continue;

		m_sceneObjects[ i ]->m_transform.update( nullptr );
	}
}

void wv::Scene::onDraw( iDeviceContext* _pContext, iLowLevelGraphics* _pDevice )
{
	Job::JobFunction_t fptr = []( const Job _job, void* _pData )
		{
			DrawData* data = (DrawData*)_pData;
			wv::iSceneObject* obj = data->pObject;

			if ( obj->m_loaded && obj->m_created )
				obj->drawImpl( data->pContext, data->pDevice );
		};

	_runJobs<DrawData>( "onUpdate", fptr, _pContext, _pDevice );
}

