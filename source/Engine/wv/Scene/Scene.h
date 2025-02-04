#pragma once

#include <wv/Decl.h>
#include <wv/Scene/SceneObject.h>
#include "wv/JobSystem/JobSystem.h"
#include <string>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class iDeviceContext;
	class iLowLevelGraphics;

///////////////////////////////////////////////////////////////////////////////////////

	class Scene
	{
	public:
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

		void addChild( IEntity* _node, bool _triggerLoadAndCreate = false );
		void removeChild( IEntity* _node );
		
		void destroyAllEntities();

		void onUpdate( double _deltaTime );

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		struct UpdateData
		{
			wv::IEntity* pObject;
			double deltaTime;
		};

		struct DrawData
		{
			wv::IEntity* pObject;
			iDeviceContext* pContext;
			iLowLevelGraphics* pDevice;
		};

		struct JobData
		{
			wv::IEntity* pObject;
			bool b;
		};

		template<typename _Ty, typename... _Args>
		void _runJobs( std::string _name, Job::JobFunction_t _fptr, _Args... _args );

		std::string m_name = "Scene";
		std::string m_sourcePath = "";

		std::vector<IEntity*> m_entities;

	};

	template<typename _Ty, typename ..._Args>
	inline void Scene::_runJobs( std::string _name, Job::JobFunction_t _fptr, _Args ..._args )
	{
		JobSystem* pJobSystem = cEngine::get()->m_pJobSystem;

		std::vector<_Ty> userDatas{ m_entities.size() };
		std::vector<Job*> jobs{};

		JobCounter* counter = nullptr;

		for ( size_t i = 0; i < m_entities.size(); i++ )
		{
			userDatas[ i ] = _Ty{ m_entities[ i ], _args... };
			Job* job = pJobSystem->createJob( _name, _fptr, &counter, &userDatas[ i ] );
			jobs.push_back( job );
		}

		pJobSystem->run( jobs.data(), jobs.size() );
		pJobSystem->waitForAndFreeCounter( &counter, 0 );
	}

}