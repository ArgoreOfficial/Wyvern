#pragma once

#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class iDeviceContext;
	class iGraphicsDevice;
	class cSceneRoot;
	class cFileSystem;

///////////////////////////////////////////////////////////////////////////////////////

	class cApplicationState
	{
	public:
		void onCreate();
		void onDestroy();

		void update( double _deltaTime );
		void draw  ( iDeviceContext* _pContext, iGraphicsDevice* _pDevice );

		void reloadScene();

		wv::cSceneRoot* loadScene( cFileSystem* _pFileSystem, const std::string& _path );

		/// <returns>scene index</returns>
		int addScene( cSceneRoot* _pScene );

		wv::cSceneRoot* getCurrentScene() { return m_pCurrentScene; }

		void switchToScene( const std::string& _name );
		void switchToScene( int _index );

///////////////////////////////////////////////////////////////////////////////////////

	private:

		std::vector<cSceneRoot*> m_scenes;

		cSceneRoot* m_pNextScene = nullptr;
		cSceneRoot* m_pCurrentScene = nullptr;

	};

}