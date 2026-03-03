#include "vtuber_controller.h"

#include <wv/application.h>
#include <wv/audio/audio_system.h>
#include <wv/entity/world.h>

#include <imgui/imgui.h>

void VtuberControllerSystem::initialize()
{
	wv::getApp()->getAudioSystem()->enableMicrophone();

	m_entity->setDebugDisplayEnabled( true );
}

void VtuberControllerSystem::shutdown()
{
	wv::getApp()->getAudioSystem()->disableMicrophone();
}

void VtuberControllerSystem::update( wv::WorldUpdateContext& _ctx )
{
	wv::AudioSystem* audioSystem = wv::getApp()->getAudioSystem();

	const float interpolationInSpeed = 1.0f / m_interpolateInTime;
	const float interpolationOutSpeed = 1.0f / m_interpolateOutTime;

	float micpeak = audioSystem->getMicrophonePeak();

	micpeak *= m_micAmplifier;

	if ( micpeak > 1.0f )
		micpeak = 1.0f;

	// Smooth Mic Value
	{
		if ( micpeak > m_currentMicValue )
			m_currentMicValue = micpeak;
		else
		{
			m_currentMicValue -= _ctx.deltaTime;
			if ( m_currentMicValue < 0.0f )
				m_currentMicValue = 0.0f;
		}
		micpeak = m_currentMicValue;
	}


	if ( micpeak < m_micThreshold )
		micpeak = 0.0f;

	m_time += _ctx.deltaTime;

	if ( micpeak > 0.0f )
	{
		if ( m_interpolation < 1.0f )
			m_interpolation += _ctx.deltaTime * interpolationInSpeed;
	}
	else
	{
		if ( m_interpolation > 0.0f )
			m_interpolation -= _ctx.deltaTime * interpolationOutSpeed;
	}

	if ( m_interpolation > 0.0f )
		m_talkingTime += _ctx.deltaTime;
	else
		m_talkingTime = 0.0f;

	m_interpolation = wv::Math::clamp( m_interpolation, 0.0f, 1.0f );

	wv::Vector3f idlePosition = { 0.0f, 0.0f, 0.0f };
	wv::Vector3f idleScale = { 1.0f, 1.0f, 1.0f };

	wv::Vector3f talkPosition = { 0.0f, 0.0f, 0.0f };
	wv::Vector3f talkScale = { 1.0f, 1.0f, 1.0f };

	// idle 
	{
		float bobbing = std::sin( m_time * m_idleBobSpeed );

		idlePosition = {
			0.0f,
			bobbing * m_idleBobAmount,
			0.0f
		};
	}

	// talking
	{
		float bobbing = std::sin( m_talkingTime * m_talkingBobSpeed );
		float squish = m_yVelocity * m_interpolation * m_squishMultiplier;
		float squishV = 1.0f - squish;
		float squishH = 1.0f + squish;

		talkPosition = {
			0.0f,
			bobbing * m_talkingBobAmount,
			0.0f
		};

		talkScale = {
			squishV,
			squishH,
			squishV
		};
	}

	wv::Vector3f newPos = wv::Math::lerp( idlePosition, talkPosition, m_interpolation );
	newPos.y = wv::Math::abs( newPos.y );

	m_yVelocity = ( newPos - m_entity->getTransform().position ).y;

	m_entity->getTransform().position = newPos;
	m_entity->getTransform().scale = talkScale;
}

void VtuberControllerSystem::onDebugRender()
{
	if ( ImGui::Begin( "Vtuber Controller System", nullptr, ImGuiWindowFlags_AlwaysAutoResize ) )
	{
		ImGui::SeparatorText( "Microphone" );
		ImGui::ProgressBar( m_currentMicValue );
		ImGui::DragFloat( "Amplifier##m_micAmplifier", &m_micAmplifier );
		ImGui::DragFloat( "Threshold##m_micThreshold", &m_micThreshold );

		ImGui::SeparatorText( "Interpolation" );
		ImGui::Text( "Current Interp: %f", m_interpolation );
		ImGui::DragFloat( "Interpolate In Time", &m_interpolateInTime, 0.025f, 0.0001f, 100.0f );
		ImGui::DragFloat( "Interpolate Out Time", &m_interpolateOutTime, 0.025f, 0.0001f, 100.0f );

		ImGui::SeparatorText( "Bobbing" );
		ImGui::DragFloat( "Idle Bob Speed", &m_idleBobSpeed );
		ImGui::DragFloat( "Idle Bob Amount", &m_idleBobAmount );

		ImGui::DragFloat( "Talking Bob Speed", &m_talkingBobSpeed );
		ImGui::DragFloat( "Talking Bob Amount", &m_talkingBobAmount );

		ImGui::SeparatorText( "Squishing" );
		ImGui::DragFloat( "Squish Multiplier", &m_squishMultiplier );

		ImGui::SeparatorText( "Backdrop" );
		wv::Transformf& bgTransform = m_backdropEntity->getTransform();

		if ( ImGui::Button( "Hide" ) )
		{
			bgTransform.position = { 0.0f, -500.0f, 0.0f };
			bgTransform.rotation = {};
		}
		
		if ( ImGui::Button( "Default" ) )
		{
			bgTransform.position = { -97.5f,  -32.0f, -57.5f };
			bgTransform.rotation = { 0.0f, -105.0f,   0.0f };
		}
	}
	ImGui::End();
}
