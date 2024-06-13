#pragma once

#include "Ship.h"

namespace wv { class Audio; }

class StarDestroyer : public Ship
{
public:
	StarDestroyer( wv::Mesh* _mesh );
	~StarDestroyer();

	void onCreate();
	void update( double _deltaTime ) override;
	void draw( wv::GraphicsDevice* _device ) override;
	void exitHyperspace( float _timer = 0.0f );

private:
	bool m_arrived = false;
	float m_arrivedTimer = 0.0f;

	float m_warpTimer = 0.0f;

	wv::Audio* m_exitHyperspaceSound = nullptr;
	bool m_playedHyperspaceSound = false;
};