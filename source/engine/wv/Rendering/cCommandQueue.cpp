#include "cCommandQueue.h"

#include <algorithm>

wv::cCommandQueue::cCommandQueue()
{
	
}

wv::cCommandQueue::~cCommandQueue()
{
}

void wv::cCommandQueue::pushStateCommand( bool _translucency, float _depth, unsigned short _material_id, void* _obj_ptr )
{
	/*
	padding      :  14
	translucency :   2
	depth        :  32
	material_id  :  16
	*/

	float d = _depth;
	
	uint64_t translucency = (uint64_t)_translucency << ( 32 + 16 ); // it's fine
	uint64_t depth        = *reinterpret_cast<uint64_t*>( &_depth );
	uint64_t material     = (uint64_t)_material_id;
	
	if ( _translucency ) depth    <<= 16; // depth priority
	else                 material <<= 16; // material priority

	uint64_t command = translucency | depth | material;
	m_queue.push_back( std::pair<uint64_t, void*>( command, _obj_ptr ) );
}

void wv::cCommandQueue::run()
{
	
}
