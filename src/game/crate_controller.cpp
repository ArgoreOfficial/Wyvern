#include "crate_controller.h"

void CrateController::onInitialize()
{
}

void CrateController::onUpdate()
{
	for ( wv::Archetype* archetype : getArchetypes() )
	{
		auto& comps = archetype->getComponents<CrateComponent>();
		auto& entities = archetype->getEntities();

		for ( size_t i = 0; i < archetype->getNumEntities(); i++ )
		{
			CrateComponent& crate = comps[ i ];
			wv::Entity* entity = entities[ i ];

			if ( !crate.cameraEntity )
				continue;

			wv::Vector3f pos = entity->getTransform().position;
			pos += crate.positionOffset;
			
			crate.cameraEntity->getTransform().position = pos;
		}
	}
}
