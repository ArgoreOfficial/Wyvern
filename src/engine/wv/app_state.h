#pragma once

#include <string>
#include <vector>
#include <queue>

#include <auxiliary/json/json11.hpp>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class IDeviceContext;
	class IGraphicsDevice;
	class Scene;
	class IFileSystem;
	class UpdateManager;
	class ICamera;
	class IUpdatable;

///////////////////////////////////////////////////////////////////////////////////////
	
	/// TODO: move

	using Json = json11::Json; // dummy

	struct ParseData
	{
		wv::Json json;
	};

	class IAppState
	{
	public:
		void initialize();
		virtual void terminate();

		virtual void onConstruct();
		virtual void onDestruct();

		virtual void onEnter();
		virtual void onExit();

		virtual void onUpdate( double _deltaTime );
		virtual void onPhysicsUpdate( double _deltaTime );

		virtual void onDraw( IDeviceContext* _pContext, IGraphicsDevice* _pDevice );

		void reloadScene();

		wv::Scene* loadScene( IFileSystem* _pFileSystem, const std::string& _path );
		void unloadScene( uint32_t _index );

		/// <returns>scene index</returns>
		int addScene( Scene* _pScene );

		wv::Scene* getCurrentScene() { return m_scenes[ m_currentScene ]; }

		void switchToScene( const std::string& _name );
		void switchToScene( int _index );

///////////////////////////////////////////////////////////////////////////////////////

		ICamera* currentCamera = nullptr;

		ICamera* orbitCamera = nullptr;
		ICamera* freeflightCamera = nullptr;

	protected:

		std::vector<Scene*> m_scenes;

		int32_t m_nextScene    = -1;
		int32_t m_currentScene = 0;

	};

	IAppState* GetAppState();
}