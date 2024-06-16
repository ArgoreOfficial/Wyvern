#pragma once

#include <wv/State/State.h>

///////////////////////////////////////////////////////////////////////////////////////

class StateGame : public wv::State
{

public:

	 StateGame();
	~StateGame();
	
	void onLoad() override;
	void onUnload() override;
	
	void onCreate() override;
	void onDestroy() override;

	void update( double _deltaTime ) override;
	void draw( wv::GraphicsDevice* _device ) override;

///////////////////////////////////////////////////////////////////////////////////////

private:



};