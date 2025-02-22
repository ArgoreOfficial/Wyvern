#pragma once

#include <wv/decl.h>
#include <wv/job/job_system.h>
#include <wv/engine.h>

#include <vector>
#include <unordered_set>
#include <queue>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

class iDeviceContext;
class IGraphicsDevice;
class cEngine;
class cApplicationState;

///////////////////////////////////////////////////////////////////////////////////////

class IUpdatable
{
public:
	
	IUpdatable() { }
	virtual ~IUpdatable();

	void _registerUpdatable();
	
	enum FunctionFlags
	{
		kOnConstruct     = 1,
		kOnDestruct      = 1 << 1,
		kOnEnter         = 1 << 2,
		kOnExit          = 1 << 3,
		kOnUpdate        = 1 << 4,
		kOnPhysicsUpdate = 1 << 5,
		kOnDraw          = 1 << 6,
	};

	virtual FunctionFlags getFunctionFlags() = 0;

	virtual void onConstruct( void ) { }
	virtual void onDestruct ( void ) { }

	virtual void onEnter( void ) { }
	virtual void onExit ( void ) { }

	virtual void onUpdate       ( double /*_deltaTime*/ ) { }
	virtual void onPhysicsUpdate( double /*_deltaTime*/ ) { }

	virtual void onDraw( wv::iDeviceContext* /*_context*/, wv::IGraphicsDevice* /*_device */) { }

};

WV_ENUM_BITWISE_OR( IUpdatable::FunctionFlags )

///////////////////////////////////////////////////////////////////////////////////////

class UpdatableOnceContainer
{
public:
	void insert( IUpdatable* _pUpdatable ) { m_awaiting.insert( _pUpdatable ); }
	void erase( IUpdatable* _pUpdatable ) {
		if( m_awaiting.count( _pUpdatable ) )
			m_awaiting.erase( _pUpdatable );
		if( m_completed.count( _pUpdatable ) )
			m_completed.erase( _pUpdatable );
	}
	void clear() {
		m_awaiting.clear();
		m_completed.clear();
	}

	void reset() {
		for( auto& u : m_completed )
			m_awaiting.insert( u );
		m_completed.clear();
	}

	void complete() {
		for( auto& u : m_awaiting )
			m_completed.insert( u );
		m_awaiting.clear();
	}

	std::unordered_set<IUpdatable*> m_awaiting;
	std::unordered_set<IUpdatable*> m_completed;
};

///////////////////////////////////////////////////////////////////////////////////////

class UpdateManager
{
public:
	friend class cEngine;
	friend class cApplicationState;

	void registerUpdatable( IUpdatable* _pUpdatable );
	void unregisterUpdatable( IUpdatable* _pUpdatable );

///////////////////////////////////////////////////////////////////////////////////////

private:
	void _registerUpdatable( IUpdatable* _pUpdatable, IUpdatable::FunctionFlags _flags );
	void _unregisterUpdatable( IUpdatable* _pUpdatable );

	void _updateQueued( void );

	template<typename _Ty, typename... _Args>
	void _runJobs( const std::unordered_set<IUpdatable*>& _set, Job::JobFunction_t _fptr, _Args... _args );

	void onConstruct( void );
	void onDestruct( void );

	void onEnter( void );
	void onExit ( void );

	void onUpdate( double _deltaTime );
	void onPhysicsUpdate( double _deltaTime );
	void onDraw( wv::iDeviceContext* _context, wv::IGraphicsDevice* _device );

	std::vector<IUpdatable*> m_updatables;

	UpdatableOnceContainer m_onConstruct;
	UpdatableOnceContainer m_onDestruct;
	
	UpdatableOnceContainer m_onEnter;
	UpdatableOnceContainer m_onExit;

	std::unordered_set<IUpdatable*> m_onUpdate;
	std::unordered_set<IUpdatable*> m_onPhysicsUpdate;
	std::unordered_set<IUpdatable*> m_onDraw;

	std::queue<IUpdatable*> m_registerQueue{};
	std::queue<IUpdatable*> m_unregisterQueue{};
};

///////////////////////////////////////////////////////////////////////////////////////

template<typename _Ty, typename ..._Args>
inline void UpdateManager::_runJobs( const std::unordered_set<IUpdatable*>& _set, Job::JobFunction_t _fptr, _Args ..._args )
{
	JobSystem* pJobSystem = cEngine::get()->m_pJobSystem;

	std::vector<_Ty> userDatas{ _set.size() };
	std::vector<Job*> jobs{};

	Fence* fence = pJobSystem->createFence();

	int i = 0;
	for ( auto& u : _set )
	{
		userDatas[ i ] = _Ty{ u, _args... };
		Job* job = pJobSystem->createJob( fence, nullptr, _fptr, &userDatas[ i ] );
		jobs.push_back( job );
		i++;
	}

	pJobSystem->submit( jobs );
	pJobSystem->waitAndDeleteFence( fence );
}

}