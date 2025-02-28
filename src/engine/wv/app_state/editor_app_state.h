#pragma once

#include <wv/app_state.h>
#include <wv/job/job_system.h>

#include <vector>

namespace wv {

struct Combo
{
	Combo() = default;
	Combo( const char* _name, const std::vector<const char*>& _options );
	int draw( float _width = 0 );

	const char* name;
	std::vector<const char*> options;
	const char* currentOption;
	int currentOptionIndex = 0;
};

class EditorAppState : public IAppState
{
public:
	EditorAppState();
	
	virtual void terminate() override;

	virtual void onDraw( IDeviceContext* _pContext, IGraphicsDevice* _pDevice ) override;

private:
	void buildPlatform();
	void buildAndRun();

	void drawBuildWindow();

	bool m_isBuilding3DS = false;
	wv::Fence* m_buildFence = nullptr;

	char m_targetAddressStr[ 16 ] = "192.168.0.160";
	int m_targetPort = 4003;

	std::vector<const char*> m_buildPlatforms = {
		"3DS",
		"PSP2"
	};

	std::vector<const char*> m_buildArchs = {
		"arm_3ds",
		"psp2"
	};

	std::vector<const char*> m_buildModes = {
		"Debug",
		"Debug-nomt",
		"Release",
		"Package"
	};

	Combo m_modeCombo;
	Combo m_platformCombo;
};

}
