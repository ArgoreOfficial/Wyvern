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

		std::string getSourcePath( void ) { return m_sourcePath; }
		std::string getName      ( void ) { return m_name; }

		void addChild( iSceneObject* _node, bool _triggerLoadAndCreate = false );
		void removeChild( iSceneObject* _node );
		
		void onLoad();
		void onUnload();

		void onCreate();
		void onDestroy();

		void onUpdate( double _deltaTime );
		void onDraw( iDeviceContext* _pContext, iLowLevelGraphics* _pDevice );

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		struct UpdateData
		{
			wv::iSceneObject* pObject;
			double deltaTime;
		};

		struct DrawData
		{
			wv::iSceneObject* pObject;
			iDeviceContext* pContext;
			iLowLevelGraphics* pDevice;
		};

		struct JobData
		{
			wv::iSceneObject* pObject;
			bool b;
		};

		template<typename _Ty, typename... _Args>
		void _runJobs( std::string _name, Job::JobFunction_t _fptr, _Args... _args );

		std::string m_name = "Scene";
		std::string m_sourcePath = "";

		std::vector<iSceneObject*> m_sceneObjects;

	};

	template<typename _Ty, typename ..._Args>
	inline void Scene::_runJobs( std::string _name, Job::JobFunction_t _fptr, _Args ..._args )
	{
		JobSystem* pJobSystem = cEngine::get()->m_pJobSystem;

		std::vector<_Ty> userDatas{ m_sceneObjects.size() };
		std::vector<JobSystem::JobID> jobs{};

		JobCounter* counter = nullptr;

		for ( size_t i = 0; i < m_sceneObjects.size(); i++ )
		{
			userDatas[ i ] = _Ty{ m_sceneObjects[ i ], _args... };
			JobSystem::JobID job = pJobSystem->createJob( _name, _fptr, &counter, &userDatas[ i ] );
			jobs.push_back( job );
		}

		pJobSystem->run( jobs.data(), jobs.size() );
		pJobSystem->waitForAndFreeCounter( &counter, 0 );
	}

}