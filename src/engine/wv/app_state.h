#pragma once

#include <string>
#include <vector>
#include <queue>

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

	class IAppState
	{
	public:
		void initialize();
		virtual void terminate();

		void registerUpdatable( IUpdatable* _pUpdatable ) {
			m_addedUpdatableQueue.push( _pUpdatable );
		}

		void unregisterUpdatable( IUpdatable* _pUpdatable ) {
			m_removedUpdatableQueue.push( _pUpdatable );
		}

		void onConstruct();
		void onDestruct();

		void onEnter();
		void onExit();

		void onUpdate( double _deltaTime );
		void onPhysicsUpdate( double _deltaTime );

		virtual void onDraw( IDeviceContext* _pContext, IGraphicsDevice* _pDevice );

		void reloadScene();

		wv::Scene* loadScene( IFileSystem* _pFileSystem, const std::string& _path );

		/// <returns>scene index</returns>
		int addScene( Scene* _pScene );

		wv::Scene* getCurrentScene() { return m_pCurrentScene; }

		void switchToScene( const std::string& _name );
		void switchToScene( int _index );

///////////////////////////////////////////////////////////////////////////////////////

		ICamera* currentCamera = nullptr;

		ICamera* orbitCamera = nullptr;
		ICamera* freeflightCamera = nullptr;

	protected:

		void _addQueued();
		void _removeQueued();

		std::vector<Scene*> m_scenes;

		Scene* m_pNextScene = nullptr;
		Scene* m_pCurrentScene = nullptr;

		std::vector<IUpdatable*> m_updatables;
		std::queue<IUpdatable*> m_addedUpdatableQueue;
		std::queue<IUpdatable*> m_removedUpdatableQueue;

	};

	IAppState* GetAppState();
}