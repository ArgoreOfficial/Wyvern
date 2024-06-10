#pragma once

#include <wv/Scene/Scene.h>

namespace wv { class Mesh; }

class SceneGame : public wv::Scene
{
public:
	 SceneGame();
	~SceneGame();

	void onLoad() override;
	void onUnload() override;

	void onCreate() override;
	void onDestroy() override;

	void update( double _deltaTime ) override;
	void draw() override;

private:

	wv::Mesh* m_skybox;
	wv::Mesh* m_player;

};