#pragma once

#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class iDeviceContext;
	class iLowLevelGraphics;
	class Scene;
	class cFileSystem;
	class UpdateManager;

///////////////////////////////////////////////////////////////////////////////////////

	class cApplicationState
	{
	public:
		void initialize();
		void terminate();

		void onConstruct();
		void onDestruct();

		void onEnter();
		void onExit();

		void onUpdate( double _deltaTime );
		void onPhysicsUpdate( double _deltaTime );

		void onDraw( iDeviceContext* _pContext, iLowLevelGraphics* _pDevice );

		void reloadScene();

		wv::Scene* loadScene( cFileSystem* _pFileSystem, const std::string& _path );

		/// <returns>scene index</returns>
		int addScene( Scene* _pScene );

		wv::Scene* getCurrentScene() { return m_pCurrentScene; }

		void switchToScene( const std::string& _name );
		void switchToScene( int _index );

		UpdateManager* getUpdateManager() { 
			return m_pUpdateManager; 
		}

///////////////////////////////////////////////////////////////////////////////////////

	private:

		std::vector<Scene*> m_scenes;

		Scene* m_pNextScene = nullptr;
		Scene* m_pCurrentScene = nullptr;

		UpdateManager* m_pUpdateManager = nullptr;
	};

	cApplicationState* getAppState();
}