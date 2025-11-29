#pragma once

#include <chrono>
#include <atomic>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace wv {

class ThreadProfiler;

class ThreadWorkTrace
{
public:
	struct StackFrame
	{
		StackFrame() = default;
		StackFrame( const StackFrame& _other ) {
			start = _other.start;
			end   = _other.end;

			depth.store( _other.depth.load() );
		}

		std::chrono::system_clock::time_point start;
		std::chrono::system_clock::time_point end;
		std::atomic_uint16_t depth;
	};

	StackFrame begin();
	void end( StackFrame& _frame );
	

	std::vector<StackFrame> getFrames() { 
		return m_frames;
	}

	void resetFrames() {
		m_frames.clear();
	}

private:
	std::atomic_uint16_t m_currentDepth;
	std::vector<StackFrame> m_frames;
};

class ThreadProfiler
{
public:
	ThreadProfiler() = default;
	~ThreadProfiler();

	void begin( void );
	void end  ( void );
	void reset( void );

	ThreadWorkTrace* getWorkTracer( std::thread::id _threadID );
	std::vector<ThreadWorkTrace::StackFrame> getStackFrames( std::thread::id _threadID );

private:
	std::chrono::system_clock::time_point m_frameStart;
	std::chrono::system_clock::time_point m_frameEnd;
	std::mutex m_mutex;

	std::unordered_map<std::thread::id, ThreadWorkTrace*> m_threadTraces;
};

}
