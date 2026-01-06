#pragma once

#include <wv/entity/entity_component.h>

namespace wv {

class PlayerInputComponent : public IEntityComponent
{
	friend class PlayerInputSystem;

	WV_REFLECT_TYPE( PlayerInputComponent )
public:
	PlayerInputComponent() = default;
	virtual ~PlayerInputComponent() { }

	void setPlayerIndex( int _index ) { m_playerIndex = _index; }
	int getPlayerIndex() const { return m_playerIndex; }

protected:

	int m_playerIndex = -1;
};

}