#pragma once

#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class IDeviceContext;
	class IGraphicsDevice;
	class Scene;
	class IFileSystem;
	class UpdateManager;

///////////////////////////////////////////////////////////////////////////////////////

	class IAppState
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

		void onDraw( IDeviceContext* _pContext, IGraphicsDevice* _pDevice );

		void reloadScene();

		wv::Scene* loadScene( IFileSystem* _pFileSystem, const std::string& _path );

		/// <returns>scene index</returns>
		int addScene( Scene* _pScene );

		wv::Scene* getCurrentScene() { return m_pCurrentScene; }

		void switchToScene( const std::string& _name );
		void switchToScene( int _index );

		UpdateManager* getUpdateManager() { 
			return m_pUpdateManager; 
		}

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		std::vector<Scene*> m_scenes;

		Scene* m_pNextScene = nullptr;
		Scene* m_pCurrentScene = nullptr;

		UpdateManager* m_pUpdateManager = nullptr;
	};

	IAppState* GetAppState();
}