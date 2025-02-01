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

///////////////////////////////////////////////////////////////////////////////////////

	class cApplicationState
	{
	public:
		void onCreate();
		void onDestroy();

		void update( double _deltaTime );
		void draw  ( iDeviceContext* _pContext, iLowLevelGraphics* _pDevice );

		void reloadScene();

		wv::Scene* loadScene( cFileSystem* _pFileSystem, const std::string& _path );

		/// <returns>scene index</returns>
		int addScene( Scene* _pScene );

		wv::Scene* getCurrentScene() { return m_pCurrentScene; }

		void switchToScene( const std::string& _name );
		void switchToScene( int _index );

///////////////////////////////////////////////////////////////////////////////////////

	private:

		std::vector<Scene*> m_scenes;

		Scene* m_pNextScene = nullptr;
		Scene* m_pCurrentScene = nullptr;

	};

}