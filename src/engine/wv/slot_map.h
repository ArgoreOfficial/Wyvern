#pragma once

#include <vector>
#include <algorithm>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv {

///////////////////////////////////////////////////////////////////////////////////////

template<typename Ty>
struct SlotMap
{
	const Ty& at( size_t _index ) const { return data.at( dataIndices.at( _index ) ); }
	      Ty& at( size_t _index )       { return data.at( dataIndices.at( _index ) ); }
	
	const Ty& operator []( size_t _index ) const { return data[ dataIndices[ _index ] ]; }
	      Ty& operator []( size_t _index )       { return data[ dataIndices[ _index ] ]; }

	size_t push( const Ty& _v ) {
		if ( data.size() == dataIDs.size() )
		{
			dataIDs.push_back( data.size() );
			dataIndices.push_back( data.size() );
		}

		data.push_back( _v );
		return dataIDs[ data.size() - 1 ];
	}

	void erase( size_t _index ) {
		size_t actualIndex    = dataIndices[ _index ];
		size_t actualEndIndex = data.size() - 1;

		if ( actualIndex == actualEndIndex ) // no swapping required
		{
			data.pop_back();
			return;
		}
		
		// swap values
		std::swap( data[ actualIndex ], data[ actualEndIndex ] );
		std::swap( dataIDs[ actualIndex ], dataIDs[ actualEndIndex ] );

		// swap virtual indices
		size_t virtualIndex     = dataIDs[ actualIndex ];
		size_t virtualSwapIndex = dataIDs[ actualEndIndex ];
		std::swap( dataIndices[ virtualIndex ], dataIndices[ virtualSwapIndex ] );

		data.pop_back();
	}

	void clear() {
		dataIndices.clear();
		dataIDs.clear();
		data.clear();
	}

	size_t size() const { return data.size(); }

	std::vector<Ty>::iterator       begin()       { return data.begin(); }
	std::vector<Ty>::const_iterator begin() const { return data.begin(); }
	std::vector<Ty>::iterator       end()       { return data.end(); }
	std::vector<Ty>::const_iterator end() const { return data.end(); }

	std::vector<size_t> dataIndices{};
	std::vector<size_t> dataIDs{};
	std::vector<Ty>     data{};
};

///////////////////////////////////////////////////////////////////////////////////////

}