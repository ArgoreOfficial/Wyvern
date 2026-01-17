#include "mesh_component.h"

#include <wv/entity/world.h>
#include <wv/filesystem/loaders/mesh_asset_loader.h>
#include <wv/debug/log.h>

wv::MeshComponent::~MeshComponent()
{
	WV_LOG_WARNING( "TODO: Destroy render mesh\n" );
}

void wv::MeshComponent::load( WorldLoadContext& _ctx )
{
	WV_ASSERT( m_state != EntityComponentState::UNLOADED );

	m_state = EntityComponentState::LOADING;

	if ( !m_path.empty() )
	{
		m_meshAsset = _ctx.meshAssetLoader->load( m_path );
	}
	else
	{
		WV_LOG_WARNING( "Mesh Component has no path\n" );
	}

	m_state = EntityComponentState::LOADED;
}

void wv::MeshComponent::unload( WorldLoadContext& _ctx )
{
	WV_ASSERT( m_state != EntityComponentState::LOADED );

	if ( m_meshAsset.isValid() )
	{
		_ctx.meshAssetLoader->unload( m_meshAsset );
	}

	m_state = EntityComponentState::UNLOADED;
}
