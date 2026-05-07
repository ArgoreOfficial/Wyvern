#include "crate_controller.h"

#include <wv/entity/world.h>
#include <wv/rendering/renderer.h>

void CrateController::onInitialize()
{
	m_playerActionGroup = updateContext->inputSystem->getActionGroup( "Player" );
	WV_ASSERT( m_playerActionGroup != nullptr );

	m_moveAction = m_playerActionGroup->getAxisActionID( "Aim" );
	m_jumpAction = m_playerActionGroup->getValueActionID( "ChargeJump" );
	m_growAction = m_playerActionGroup->getValueActionID( "Grow" );
}

void CrateController::onUpdate()
{
	for ( wv::Archetype* archetype : getArchetypes() )
	{
		auto& crates = archetype->getComponents<CrateComponent>();
		auto& rbs = archetype->getComponents<wv::RigidBodyComponent>();
		auto& entities = archetype->getEntities();

		for ( size_t i = 0; i < archetype->getNumEntities(); i++ )
		{
			CrateComponent& crate = crates[ i ];
			wv::RigidBodyComponent& rb = rbs[ i ];
			wv::Entity* entity = entities[ i ];

			if ( !crate.cameraEntity )
				continue;

			bool grow = false;

			// Grab inputs

			for ( auto& ev : updateContext->actionEventQueue )
			{
				if ( ev.actionID == m_jumpAction )
				{
					if( crate.charge >= 0.0f )
						crate.charge = ev.getValue();
				}
				if ( ev.actionID == m_moveAction )
				{
					crate.currentAim.x = ev.getAxis().x;
					crate.currentAim.z = -ev.getAxis().y;
					crate.currentAim.y = 1.0f;
				}
			}

			crate.currentAim.normalize();

			// Camera update

			wv::Vector3f pos = entity->getTransform().position;
			pos += crate.positionOffset;
			
			crate.cameraEntity->getTransform().position = pos;

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

			entity->getTransform().scale = wv::Vector3f{ 1.0f, 1.0f, 1.0f } + crate.jumpScale * jumpPower * crate.visualChargeScale;
		}
	}
}
