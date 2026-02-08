#include "world.h"

#include <wv/application.h>
#include <wv/camera/view_volume.h>
#include <wv/rendering/viewport.h>
#include <wv/input/input_system.h>

#include <wv/rendering/material.h>
#include <wv/rendering/mesh.h>
#include <wv/rendering/texture.h>

#include <wv/thread/task_system.h>

wv::World::World()
{ 
	m_meshManager     = WV_NEW( MeshManager );
	m_materialManager = WV_NEW( MaterialManager );
	m_textureManager  = WV_NEW( TextureManager );
}

wv::World::~World()
{
	m_meshManager->clearPersistent();
	m_materialManager->clearPersistent();
	m_textureManager->clearPersistent();

	if ( m_viewport )
		WV_FREE( m_viewport );

	// free managers
	WV_FREE( m_meshManager );
	WV_FREE( m_materialManager );
	WV_FREE( m_textureManager );
}

void wv::World::shutdown()
{
	// Unload and shutdown

	WorldLoadContext ctx = getLoadContext();

	for ( WorldSector* sector : m_sectors )
		sector->unload( ctx );
	
	for ( auto system : m_systems )
		system->shutdown();
	
	for ( IWorldSystem* system : m_systems ) WV_FREE( system );
	for ( WorldSector*  sector : m_sectors ) WV_FREE( sector );
	
	m_sectors.clear();
	m_systems.clear();
}

void wv::World::addSector( WorldSector* _sector )
{
	if ( findSector( _sector->getID() ) != nullptr )
	{
		WV_LOG_ERROR( "World already contains sector %zu", _sector->getID() );
		return;
	}

	WV_ASSERT_MSG( _sector->m_parentWorld == nullptr, "Sector already has a parent world" );
	
	_sector->m_parentWorld = this;

	m_sectors.push_back( _sector );
	m_sectorMap.emplace( _sector->getID(), _sector );
}

void wv::World::destroySector( WorldSectorID _sectorID )
{
	WorldSector* sector = findSector( _sectorID );
	if ( sector== nullptr )
	{
		WV_LOG_ERROR( "World does not contain sector %zu", _sectorID );
		return;
	}

	WV_LOG_ERROR( "Sector unload and shutdown\n" );

	sector->m_parentWorld = nullptr;

	m_sectorMap.erase( _sectorID );

	for ( size_t i = 0; i < m_sectors.size(); i++ )
	{
		if ( m_sectors[ i ] != sector )
			continue;

		m_sectors.erase( m_sectors.begin() + i );
		break;
	}
}

void wv::World::updateLoading()
{
	WorldLoadContext ctx = getLoadContext();

	for ( auto sector : m_sectors )
		sector->updateLoading( ctx );
}

void wv::World::updateSectors( double _deltaTime )
{
	WorldUpdateContext ctx{};
	ctx.viewport = m_viewport;
	ctx.deltaTime = _deltaTime;
	ctx.inputSystem = wv::Application::getSingleton()->getInputSystem();
	ctx.actionEventQueue = ctx.inputSystem->getActionEventQueue();

	for ( auto sector : m_sectors )
		sector->update( ctx );
}

void wv::World::updateWorldSystems( double _deltaTime )
{
	WorldUpdateContext ctx{};
	ctx.viewport = m_viewport;
	ctx.deltaTime = _deltaTime;
	ctx.inputSystem = wv::Application::getSingleton()->getInputSystem();
	ctx.actionEventQueue = ctx.inputSystem->getActionEventQueue();

	for ( auto system : m_systems )
		system->update( ctx );

	if( ViewVolume* viewVolume = m_viewport->getViewVolume() )
		viewVolume->recalculateViewMatrix();
}

wv::WorldLoadContext wv::World::getLoadContext()
{
	WorldLoadContext ctx{};
	ctx.meshManager     = m_meshManager;
	ctx.materialManager = m_materialManager;
	ctx.inputSystem     = wv::Application::getSingleton()->getInputSystem();
	ctx.textureManager  = m_textureManager;
	return ctx;
}

void wv::World::createWorldSystem( IWorldSystem* _system )
{
	if ( _system == nullptr )
		return;

	m_systems.push_back( _system );

	_system->initialize();

	for ( auto sector : m_sectors )
	{
		for ( auto entity : sector->getEntities() )
		{
			
			for ( auto component : entity->getComponents() )
				_system->registerComponent( entity, component );
		}
	}
}
