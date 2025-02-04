#include "Scene.h"

#include <wv/Memory/Memory.h>

///////////////////////////////////////////////////////////////////////////////////////

void wv::Scene::addChild( IEntity* _node, bool _triggerLoadAndCreate )
{
	if ( !_node )
		return;

	for ( size_t i = 0; i < m_entities.size(); i++ )
		if ( m_entities[ i ] == _node ) return; // node already has child
	
	m_entities.push_back( _node );

	if ( _triggerLoadAndCreate )
	{
		_node->onEnterImpl();
		_node->onConstructImpl();
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Scene::removeChild( IEntity* _node )
{
	if ( !_node )
		return;

	for ( size_t i = 0; i < m_entities.size(); i++ )
	{
		if ( m_entities[ i ] != _node )
			continue;

		m_entities.erase( m_entities.begin() + i );
		return;
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Scene::onConstruct()
{
	Job::JobFunction_t fptr = []( const Job* _job, void* _pData )
		{
			JobData* data = (JobData*)_pData;
			wv::IEntity* obj = data->pObject;
			if ( !obj->m_loaded )
			{
				obj->onConstructImpl();
				obj->m_loaded = true;
			}
		};

	_runJobs<JobData>( "onConstruct", fptr, false );
}

void wv::Scene::onDeconstruct()
{
	Job::JobFunction_t fptr = []( const Job* _job, void* _pData )
		{
			JobData* data = (JobData*)_pData;
			wv::IEntity* obj = data->pObject;
			if ( obj->m_loaded )
			{
				obj->onDeconstructImpl();
				obj->m_loaded = false;
			}
		};

	_runJobs<JobData>( "onDeconstruct", fptr, false );

	for( size_t i = 0; i < m_entities.size(); i++ )
		WV_FREE( m_entities[ i ] );

}

void wv::Scene::onEnter()
{
	Job::JobFunction_t fptr = []( const Job* _job, void* _pData )
		{
			JobData* data = (JobData*)_pData;
			wv::IEntity* obj = data->pObject;
			if ( !obj->m_created )
			{
				obj->onEnterImpl();
				obj->m_created = true;
			}
		};

	_runJobs<JobData>( "onEnter", fptr, false );
}

void wv::Scene::onExit()
{
	Job::JobFunction_t fptr = []( const Job* _job, void* _pData )
		{
			JobData* data = (JobData*)_pData;
			wv::IEntity* obj = data->pObject;
			if ( obj->m_created )
			{
				obj->onExitImpl();
				obj->m_created = false;
			}
		};

	_runJobs<JobData>( "onExit", fptr, false );
}

void wv::Scene::onUpdate( double _deltaTime )
{
	Job::JobFunction_t fptr = []( const Job* _job, void* _pData )
		{
			UpdateData* updateData = (UpdateData*)_pData;
			wv::IEntity* obj = updateData->pObject;

			if ( obj->m_loaded && obj->m_created )
				obj->onUpdate( updateData->deltaTime );
		};

	_runJobs<UpdateData>( "onUpdate", fptr, _deltaTime );

	for ( size_t i = 0; i < m_entities.size(); i++ )
	{
		if ( m_entities[ i ]->m_transform.pParent != nullptr )
			continue;

		m_entities[ i ]->m_transform.update( nullptr );
	}
}

void wv::Scene::onDraw( iDeviceContext* _pContext, iLowLevelGraphics* _pDevice )
{
	Job::JobFunction_t fptr = []( const Job* _job, void* _pData )
		{
			DrawData* data = (DrawData*)_pData;
			wv::IEntity* obj = data->pObject;

			if ( obj->m_loaded && obj->m_created )
				obj->drawImpl( data->pContext, data->pDevice );
		};

	_runJobs<DrawData>( "onUpdate", fptr, _pContext, _pDevice );
}

