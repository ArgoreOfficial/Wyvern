#include "thread_profiler.h"

#include <wv/memory/memory.h>

wv::ThreadWorkTrace::StackFrame wv::ThreadWorkTrace::begin()
{
	StackFrame frame;
	frame.depth = m_currentDepth++;
	frame.start = std::chrono::system_clock::now();
	return frame;
}

void wv::ThreadWorkTrace::end( StackFrame& _frame )
{
	_frame.end = std::chrono::system_clock::now();
	m_frames.push_back( _frame );

	m_currentDepth--;
}

wv::ThreadProfiler::~ThreadProfiler()
{
	for( auto t : m_threadTraces )
		WV_FREE( t.second );
	m_threadTraces.clear();
}

void wv::ThreadProfiler::begin()
{
	m_mutex.lock();
	m_frameStart = std::chrono::system_clock::now();
}

void wv::ThreadProfiler::end()
{
	m_frameEnd = std::chrono::system_clock::now();
	m_mutex.unlock();
}

void wv::ThreadProfiler::reset( void )
{
	for( auto& t : m_threadTraces )
		t.second->resetFrames();
}

wv::ThreadWorkTrace* wv::ThreadProfiler::getWorkTracer( std::thread::id _threadID )
{
	if( m_threadTraces.count( _threadID ) == 0 )
		m_threadTraces[ _threadID ] = WV_NEW( ThreadWorkTrace );
	
	return m_threadTraces[ _threadID ];
}

std::vector<wv::ThreadWorkTrace::StackFrame> wv::ThreadProfiler::getStackFrames( std::thread::id _threadID )
{
	std::scoped_lock lock{ m_mutex };
	return m_threadTraces[ _threadID ]->getFrames();
}
