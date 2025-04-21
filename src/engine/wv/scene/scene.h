#pragma once

#include <wv/decl.h>
#include <wv/scene/scene_object.h>
#include <wv/job/job_system.h>

#include <wv/scene/iupdatable.h>

#include <string>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class IDeviceContext;
	class IGraphicsDevice;
	class IAppState;

///////////////////////////////////////////////////////////////////////////////////////

	class Scene
	{
	public:

		friend class IAppState;

		Scene( const std::string& _name, const std::string& _sourcePath = "" ) :
			m_name{ _name },
			m_sourcePath{ _sourcePath }
		{
		}

		~Scene() {
			destroyAllEntities();
		}
		
		std::string getSourcePath( void ) { return m_sourcePath; }
		std::string getName      ( void ) { return m_name; }
		
		std::vector<IEntity*> getEntities( void ) const { return m_entities; }

		void registerUpdatable( IUpdatable* _pUpdatable ) {
			m_addedUpdatableQueue.push( _pUpdatable );
			_pUpdatable->m_pRegisteredScene = this;
		}

		void unregisterUpdatable( IUpdatable* _pUpdatable ) {
			m_removedUpdatableQueue.push( _pUpdatable );
			_pUpdatable->m_pRegisteredScene = nullptr;
		}

		void addChild( IEntity* _node, bool _triggerLoadAndCreate = false );
		void removeChild( IEntity* _node );

		void onConstruct();
		void onDestruct();

		void onEnter();
		void onExit();

		void onUpdate( double _deltaTime );
		void onPhysicsUpdate( double _deltaTime );

		void onDraw( IDeviceContext* _pContext, IGraphicsDevice* _pDevice );

		void destroyAllEntities();

		void onUpdateTransforms();

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		void _addQueued();
		void _removeQueued();

		std::string m_name = "Scene";
		std::string m_sourcePath = "";

		std::vector<IEntity*> m_entities;

		std::vector<IUpdatable*> m_updatables;
		std::queue<IUpdatable*> m_addedUpdatableQueue;
		std::queue<IUpdatable*> m_removedUpdatableQueue;

	};

}