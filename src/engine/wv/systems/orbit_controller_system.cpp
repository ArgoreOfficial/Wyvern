#include "orbit_controller_system.h"

#include <wv/components/orbit_controller_component.h>
#include <wv/application.h>
#include <wv/input/input_system.h>

void wv::OrbitControllerSystem::configure( ArchetypeConfig& _config )
{
	_config.addComponentType<OrbitControllerComponent>();
}

void wv::OrbitControllerSystem::onUpdate()
{
	InputSystem* inputSystem = getApp()->getInputSystem();
	double deltaTime = getApp()->getDeltaTime();

	for ( Archetype* archetype : getArchetypes() )
	{
		auto& controllers = archetype->getComponents<OrbitControllerComponent>();
		auto& entities    = archetype->getEntities();

		for ( size_t i = 0; i < archetype->getNumEntities(); i++ )
		{
			OrbitControllerComponent& component = controllers[ i ];
			Entity* entity = entities[ i ];

			if ( inputSystem->getMouseButtonState( 2 ) )
			{
				component.cameraMove = {};

				Vector2i motion = inputSystem->getMouseMotion();
				component.cameraMove.x += 0.4f * (float)motion.x;
				component.cameraMove.y += 0.4f * (float)motion.y;
			}
			else
			{
				const float friction = 5.f;
				const float frictionDecay = 1 / ( 1 + ( deltaTime * friction ) );

				component.cameraMove *= frictionDecay;
			}

			component.orbitDistance -= inputSystem->getMouseScroll() * 120 * deltaTime;

			wv::Transformf& entityTransform = entity->getTransform();

			// check if camera is orbit camera
			{
				entityTransform.rotation += wv::Vector3f{
						-component.cameraMove.y,
						-component.cameraMove.x,
						0.0f
				};

				entityTransform.setPosition( entityTransform.rotation.eulerToDirection() * component.orbitDistance );
			}
		}
	}

	
}
