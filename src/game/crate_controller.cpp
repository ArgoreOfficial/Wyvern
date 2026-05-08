#include "crate_controller.h"

#include <wv/entity/world.h>
#include <wv/rendering/renderer.h>
#include <wv/systems/physics_system.h>

void CrateController::onInitialize()
{
	m_playerActionGroup = updateContext->inputSystem->getActionGroup( "Player" );
	WV_ASSERT( m_playerActionGroup != nullptr );

	m_moveAction = m_playerActionGroup->getAxisActionID( "Aim" );
	m_jumpAction = m_playerActionGroup->getValueActionID( "ChargeJump" );
}

void CrateController::onUpdate()
{
	std::vector<wv::Vector3f> cratePositions;
	wv::Entity* cameraEntity = nullptr;
	wv::Vector3f cameraOffset{};

	wv::PhysicsSystem* physics = getWorld()->getSystem<wv::PhysicsSystem>();

	for ( wv::Archetype* archetype : getArchetypes() )
	{
		auto& crates = archetype->getComponents<CrateComponent>();
		auto& rbs = archetype->getComponents<wv::RigidBodyComponent>();
		auto& playerInputs = archetype->getComponents<wv::PlayerInputComponent>();
		auto& entities = archetype->getEntities();

		for ( size_t i = 0; i < archetype->getNumEntities(); i++ )
		{
			CrateComponent& crate = crates[ i ];
			wv::RigidBodyComponent& rb = rbs[ i ];
			wv::Entity* entity = entities[ i ];

			cratePositions.push_back( entity->getTransform().position );

			if ( crate.cameraEntity )
			{
				cameraEntity = crate.cameraEntity;
				cameraOffset = crate.positionOffset;
			}
			
			bool grow = false;
			bool grounded = false;

			wv::RaycastHit hit;
			if ( physics->sphereCast( entity->getTransform().position, 0.5f, { 0.0f, -0.15f, 0.0f }, hit, { wv::PhysicsLayer_NonMoving } ) )
				grounded = true;
			
			// Grab inputs

			for ( auto& ev : updateContext->actionEventQueue )
			{
				if ( ev.playerIndex != playerInputs[ i ].playerIndex )
					continue;

				if ( ev.actionID == m_jumpAction )
				{
					if( crate.charge >= 0.0f && grounded )
						crate.charge = ev.getValue( ev.playerIndex );
				}
				if ( ev.actionID == m_moveAction )
				{
					crate.currentAim.x = ev.getAxis( ev.playerIndex ).x;
					crate.currentAim.z = -ev.getAxis( ev.playerIndex ).y;
					crate.currentAim.y = 1.0f;
				}
			}

			crate.currentAim.normalize();

			// Move update

			wv::getApp()->getRenderer()->addDebugLine(
				entity->getTransform().position,
				entity->getTransform().position + crate.currentAim
			);

			crate.currentJumpCharge += crate.charge * deltaTime;

			if ( crate.currentJumpCharge <= 0.0f )
			{
				crate.currentJumpCharge = 0.0f;
				crate.charge = 0.0f;
			}

			float jumpPower = 1.0f - std::powf( 2.0f, -crate.jumpForceExponent * crate.currentJumpCharge );

			if ( crate.currentJumpCharge > 0.001f && crate.charge == 0.0f )
			{
				rb.addForce( crate.currentAim * jumpPower * crate.jumpForceMultiplier, wv::ForceType_Impulse );
				rb.addTorque( wv::Vector3f{ crate.currentAim.z, 0.0f, -crate.currentAim.x } * jumpPower * crate.jumpTorqueMultiplier );

				crate.charge = -30.0f;
			}

			// Visual Update

			// currently is charging
			if ( crate.currentJumpCharge > 0.001f && crate.charge > 0.0f )
			{
				wv::Vector3f up{ 0.0f, 1.0f, 0.0f };;

				float x = std::abs( up.dot( entity->getTransform().right() ) );
				float y = std::abs( up.dot( entity->getTransform().up() ) );
				float z = std::abs( up.dot( entity->getTransform().forward() ) );

				if ( x >= y && x >= z ) 
					crate.jumpScale = { -1.0f, 1.0f, 1.0f };
				else if ( y >= x && y >= z ) 
					crate.jumpScale = { 1.0f, -1.0f, 1.0f };
				else 
					crate.jumpScale = { 1.0f, 1.0f, -1.0f };
			}

			if ( crate.meshEntity )
			{
				crate.meshEntity->getTransform().scale = wv::Vector3f{ 1.0f, 1.0f, 1.0f } + crate.jumpScale * jumpPower * crate.visualChargeScale;

				if ( crate.charge > 0.0f && jumpPower > 0.6f )
				{
					float shake = ( jumpPower - 0.6f ) * 0.1f;
					crate.meshEntity->getTransform().position = {
						wv::Math::randomRange( -shake, shake ),
						wv::Math::randomRange( -shake, shake ),
						wv::Math::randomRange( -shake, shake )
					};
				}
				else
				{
					crate.meshEntity->getTransform().position = {};
				}
			}
			
		}
	}

	if ( !cameraEntity )
		return;

	wv::Vector3f centrePosition{};
	float height = 0.0f;
	float cameraDistance = 5.0f;

	for ( auto& p : cratePositions )
		centrePosition += p;
	centrePosition /= cratePositions.size();

	for ( auto& p : cratePositions )
		cameraDistance = wv::Math::max( cameraDistance, ( p - centrePosition ).length() );
	
	cameraEntity->getTransform().position = centrePosition + cameraOffset * cameraDistance * 1.5f;

}
