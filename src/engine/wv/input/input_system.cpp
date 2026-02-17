#include "input_system.h"

#include <wv/application.h>
#include <wv/event/event_manager.h>
#include <wv/memory/memory.h>

#include <wv/input/drivers/controller_driver.h>
#include <wv/input/drivers/input_driver.h>
#include <wv/input/drivers/mouse_driver.h>

#include <wv/platform/platform.h>

wv::InputSystem::InputSystem()
{

}

wv::InputSystem::~InputSystem()
{
	for ( IInputDriver* driver : m_inputDrivers )
		WV_FREE( driver );

	for ( auto actionGroup : m_actionGroups )
		WV_FREE( actionGroup );

	m_inputDrivers.clear();

	m_actionGroups.clear();
	m_actionGroupNameMap.clear();
	m_actionEventQueue.clear();

	m_virtualDevices.clear();
}

void wv::InputSystem::initialize()
{
	for ( IInputDriver* driver : m_inputDrivers )
		driver->initialize( this );
}

void wv::InputSystem::shutdown()
{
	for ( IInputDriver* driver : m_inputDrivers )
		driver->shutdown( this );
}


void wv::InputSystem::processInputEvents( EventManager* _eventManager )
{
	m_actionEventQueue.clear();

	m_lowLevelInputQueue.clearQueue();

	wv::Platform::pollEvents( m_lowLevelInputQueue );

	for ( IInputDriver* driver : m_inputDrivers )
		driver->pollActions( this );

	if ( IMouseDriver* mouseDriver = getInputDriver<IMouseDriver>() )
	{
		m_mouseState     = mouseDriver->getMouseState();
		m_prevMouseState = mouseDriver->getPrevMouseState();
	}

	
}

void wv::InputSystem::setMotorSpeed( uint32_t _vdID, uint16_t _left, uint16_t _right )
{
	IControllerDriver* controllerDriver = nullptr;
	for ( IInputDriver* driver : m_inputDrivers )
	{
		if ( driver->getDriverType() != "Controller" )
			continue;
		controllerDriver = static_cast<IControllerDriver*>( driver );
	}

	if ( controllerDriver == nullptr )
		return;

	controllerDriver->setMotorSpeed( _vdID, _left, _right, 0 );
}

void wv::InputSystem::setDevicePlayer( uint32_t _vdID, int _playerIndex )
{
	if ( _playerIndex < -1 )
		_playerIndex = -1;

	bool requiresNew = true;
	int prevPlayerIndex = -1;

	for ( auto& device : m_virtualDevices )
	{
		if ( device.virtualDeviceID != _vdID )
			continue;

		prevPlayerIndex = device.playerIndex;
		device.playerIndex = _playerIndex;
		requiresNew = false;
		break;
	}

	// create new virtual device
	if ( requiresNew )
		m_virtualDevices.emplace_back( _vdID, _playerIndex );

	for ( auto& action : m_actionEventQueue )
	{
		if ( action.vdID != _vdID )
			continue;

		if ( action.playerIndex == _playerIndex )
			continue;

		action.playerIndex = _playerIndex;

		switch ( action.type )
		{
		case ACTION_TYPE_TRIGGER: action.action.trigger->setValue( _playerIndex, action.action.trigger->getValue( prevPlayerIndex ) ); break;
		case ACTION_TYPE_VALUE:   action.action.value->setValue( _playerIndex, action.action.value->getValue( prevPlayerIndex ) ); break;
		case ACTION_TYPE_AXIS:    action.action.axis->setValue( _playerIndex, action.action.axis->getValue( prevPlayerIndex ) ); break;
		}
	}
}

int wv::InputSystem::getDevicePlayer( uint32_t _vdID )
{
	for ( auto& device : m_virtualDevices )
	{
		if ( device.virtualDeviceID != _vdID )
			continue;

		return device.playerIndex;
	}

	return -1;
}

wv::ActionGroup* wv::InputSystem::createActionGroup( const std::string& _name )
{
	if ( m_actionGroupNameMap.contains( _name ) && m_actionGroupNameMap.at( _name ) != nullptr )
	{
		WV_LOG_WARNING( "ActionGroup '%s' already exists, a new one will not be created\n", _name.c_str() );
		return m_actionGroupNameMap.at( _name );
	}

	ActionGroup* group = WV_NEW( ActionGroup, _name );
	m_actionGroups.push_back( group );
	m_actionGroupNameMap.emplace( _name, group );
	return group;
}

void wv::InputSystem::destroyActionGroup( const std::string& _name )
{
	if ( !m_actionGroupNameMap.contains( _name ) )
	{
		WV_LOG_WARNING( "Cannot delete action group '%s'. it does not exist\n", _name.c_str() );
		return;
	}

	ActionGroup* group = m_actionGroupNameMap.at( _name );
	m_actionGroupNameMap.erase( _name );

	for ( auto it = m_actionGroups.begin(); it != m_actionGroups.end(); )
	{
		if ( *it != group )
			continue;

		m_actionGroups.erase( it );
		break;
	}

	WV_FREE( group );
}
