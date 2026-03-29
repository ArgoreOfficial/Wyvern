#pragma once

#include <wv/reflection/reflection.h>

#include <wv/entity/world_sector.h>

#include <wv/math/vector2.h>
#include <wv/input/input_system.h>

namespace wv {

class WorldSector;
class ViewVolume;
class Viewport;
class InputSystem;

class MeshManager;
class MaterialManager;
class TextureManager;

struct WorldLoadContext
{
	InputSystem* inputSystem = nullptr;
	
	MeshManager*     meshManager     = nullptr;
	MaterialManager* materialManager = nullptr;
	TextureManager*  textureManager  = nullptr;
};

struct WorldUpdateContext
{
	Viewport* viewport = nullptr;
	double deltaTime = 0.0;
	
	InputSystem* inputSystem = nullptr;
	std::vector<ActionEvent> actionEventQueue;
};

class World : public IReflectedType
{
	friend class Application;
	WV_REFLECT_TYPE( World, IReflectedType )
public:
	World();
	virtual ~World();

	void shutdown();

	WorldSector* findSector( UUID _UUID ) const {
		auto it = m_sectorMap.find( _UUID );
		if ( it == m_sectorMap.end() )
			return nullptr;
		return it->second;
	}

	Entity* findFirstEntityByName( const std::string& _name ) const {
		for ( auto sector : m_sectors )
		{
			for ( auto entity : sector->getEntities() )
			{
				if ( entity->getName() == _name )
					return entity;
			}
		}

		return nullptr;
	}

	void addSector( WorldSector* _sector );
	void destroySector( UUID _sectorID );

	void updateLoading();
	void updateSectors( double _deltaTime );
	
	MeshManager*     getMeshManager()     const { return m_meshManager; }
	MaterialManager* getMaterialManager() const { return m_materialManager; }
	TextureManager*  getTextureManager()  const { return m_textureManager; }

	Viewport* getViewport() const                { return m_viewport; }
	void      setViewport( Viewport* _viewport ) { m_viewport = _viewport; }

	WorldLoadContext getLoadContext();

protected:
	virtual void onSceneCreate() { }
	virtual void onSetupInput( InputSystem* _inputSystem ) { }

	std::vector<WorldSector*> m_sectors;
	std::vector<WorldSector*> m_sectorsToLoad;
	std::unordered_map<UUID, WorldSector*> m_sectorMap;
	
	Viewport* m_viewport = nullptr;

	MeshManager*     m_meshManager     = nullptr;
	MaterialManager* m_materialManager = nullptr;
	TextureManager*  m_textureManager  = nullptr;
};

}