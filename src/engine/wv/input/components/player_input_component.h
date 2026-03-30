#pragma once

namespace wv {

struct PlayerInputComponent
{
	void setPlayerIndex( int _index ) { m_playerIndex = _index; }
	int getPlayerIndex() const { return m_playerIndex; }

	int m_playerIndex = -1;
};

}