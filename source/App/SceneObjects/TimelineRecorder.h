#pragma once

#include <vector>

namespace psq
{
	class iStateRecordBase
	{
	public:
		virtual void setFrame( int _index ) = 0;
		virtual void rewindState  ( void )       = 0;
		virtual void recordState  ( void )       = 0;

		virtual size_t getRecordSize() = 0;
	};

	template<typename T>
	class cStateRecord : public iStateRecordBase
	{
	public:
		cStateRecord( T* _variable, size_t _size ) : 
			m_pVariable{ _variable } 
		{
			m_recordedStates.resize( _size );
		}

		virtual void setFrame( int _index );

		virtual void rewindState() override
		{
			if( m_currentFrame == 0 )
				setFrame( m_currentFrame ); // don't change
			else
				setFrame( m_currentFrame - 1 );
		}

		virtual void recordState() override;
		virtual size_t getRecordSize() override { return m_recordedStates.size(); }
		
	private:

		size_t m_currentFrame = 0;
		size_t m_validFrames  = 0;

		T* m_pVariable = nullptr;

		std::vector<T> m_recordedStates{};

	};

	class cTimelineRecorder
	{
	public:

		cTimelineRecorder( size_t _size = 512 ) :
			m_timelineSize{ _size }
		{ }

		~cTimelineRecorder( void ) { }
		
		void setRewinding ( bool _isRewinding ) { m_isRewinding = _isRewinding; }
		void setEnabled   ( bool _enabled )     { m_enabled     = _enabled; };
		void setFrameSpeed( int _speed )        { m_frameSpeed  = _speed; }
		void setFrame     ( int _frame );
		
		size_t getTotalFrames();

		void update();

		template<typename T>
		void startRecordingVariable( T* _var )
		{
			cStateRecord<T>* var = new cStateRecord<T>( _var, m_timelineSize );
			m_recordingVariables.push_back( var );
		}

	private:
		bool m_isRewinding = false;
		bool m_enabled     = false;
		int  m_frameSpeed  = 1;

		size_t m_timelineSize = 0;
		std::vector<iStateRecordBase*> m_recordingVariables;

	};

	template<typename T>
	inline void cStateRecord<T>::setFrame( int _index )
	{
		if( _index < 0 || _index >= m_validFrames )
			return;

		*m_pVariable = m_recordedStates[ _index ];
		m_currentFrame = _index;
	}

	template<typename T>
	inline void cStateRecord<T>::recordState()
	{
		if( m_currentFrame < m_recordedStates.size() - 1 )
			m_recordedStates[ m_currentFrame ] = *m_pVariable;
		else
			m_recordedStates.push_back( *m_pVariable );

		m_currentFrame++;
		m_validFrames = m_currentFrame;
	}
}