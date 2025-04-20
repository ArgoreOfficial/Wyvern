#pragma once

#include <wv/app.h>

class Sandbox : public wv::IApplication
{
public:
	Sandbox() { }

	// Inherited via IApplication
	bool create ( void ) override;
	void run    ( void ) override;
	void destroy( void ) override;

	void initImgui();
	void shutdownImgui();

	void tick();

	void refreshFPSDisplay( double _deltaTime );

	unsigned int m_fpsCounter = 0;
	double m_fpsAccumulator = 0.0;
	double m_fpsUpdateInterval = 0.5;
	double m_timeSinceFPSUpdate = 0.0;

	double m_averageFps = 0.0;
	double m_maxFps = 0.0;

};