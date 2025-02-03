#pragma once

#include <wv/Decl.h>

#include <vector>
#include <unordered_set>

namespace wv
{

class iDeviceContext;
class iLowLevelGraphics;
class cEngine;
class cApplicationState;

class IUpdatable
{
public:
	
	IUpdatable() { }
	virtual ~IUpdatable();

	void registerUpdatable();
	
	enum FunctionFlags
	{
		kOnLoad     = 1,
		kOnUnload   = 1 << 1,
		kOnEnter    = 1 << 2,
		kOnExit     = 1 << 3,
		kOnUpdate   = 1 << 4,
		kOnDraw     = 1 << 5
	};

	virtual FunctionFlags getFunctionFlags() = 0;

	virtual void onLoad  ( void ) { };
	virtual void onUnload( void ) { };

	virtual void onEnter( void ) { };
	virtual void onExit ( void ) { };

	virtual void onUpdate( double _deltaTime ) { };
	virtual void onDraw( wv::iDeviceContext* _context, wv::iLowLevelGraphics* _device ) { };

};

WV_ENUM_BITWISE_OR( IUpdatable::FunctionFlags )

class UpdateManager
{
public:
	friend class cEngine;
	friend class cApplicationState;

	void registerUpdatable( IUpdatable* _pUpdatable, IUpdatable::FunctionFlags _flags );
	void removeUpdatable( IUpdatable* _pUpdatable );

private:

	void onLoad( void );
	void onUnload( void );

	void onEnter( void );
	void onExit ( void );

	void onUpdate( double _deltaTime );
	void onDraw( wv::iDeviceContext* _context, wv::iLowLevelGraphics* _device );

	std::vector<IUpdatable*> m_updatables;

	std::unordered_set<IUpdatable*> m_onLoad;
	std::unordered_set<IUpdatable*> m_onUnload;
	
	std::unordered_set<IUpdatable*> m_onEnter;
	std::unordered_set<IUpdatable*> m_onExit;

	std::unordered_set<IUpdatable*> m_onUpdate;
	std::unordered_set<IUpdatable*> m_onDraw;
};

}