#pragma once

#include <vector>

namespace wv
{
	class cCommandQueue
	{
	public:

		 cCommandQueue();
		~cCommandQueue();

		void pushStateCommand( bool _translucency, float _depth, unsigned short _material_id, void* _obj_ptr );

		void run();

	private:

		/*
		padding      :  14
		translucency :   2
		depth        :  32
		material_id  :  16
		*/
		std::vector<std::pair<uint64_t, void*>> m_queue;

	};
}