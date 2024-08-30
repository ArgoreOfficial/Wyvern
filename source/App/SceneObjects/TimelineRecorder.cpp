#include "TimelineRecorder.h"

void psq::cTimelineRecorder::setFrame( int _frame )
{
	for( auto& rvar : m_recordingVariables )
		rvar->setFrame( _frame );
}

size_t psq::cTimelineRecorder::getTotalFrames()
{
	size_t max = 0;
	for( auto& rvar : m_recordingVariables )
	{
		size_t localMax = rvar->getRecordSize();
		if( localMax > max )
			max = localMax;
	}
	return max;
}

void psq::cTimelineRecorder::update()
{
	if( !m_enabled )
		return;

	for( int i = 0; i < m_frameSpeed; i++ )
	{
		for( auto& rvar : m_recordingVariables )
		{
			if( m_isRewinding )
				rvar->rewindState();
			else
				rvar->recordState();
		}
	}

}
