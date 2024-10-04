#pragma once

#include <functional>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

    template<typename T, typename Tag>
    struct sStrongID
    {
        sStrongID( void )        : value{ InvalidID } { }
        sStrongID( const T& _v ) : value{ _v }        { }

        static constexpr T InvalidID = 0;
        T value = InvalidID;

        inline void invalidate() { value = InvalidID; }
        inline bool isValid() const { return value != InvalidID; }

        bool operator==( const sStrongID<T, Tag>& _other ) const { return value == _other.value; }
        bool operator> ( const sStrongID<T, Tag>& _other ) const { return value > _other.value; }
        bool operator< ( const sStrongID<T, Tag>& _other ) const { return value < _other.value; }
    };

}

///////////////////////////////////////////////////////////////////////////////////////

namespace std
{

	template <typename T, typename Tag>
	struct hash<wv::sStrongID<T, Tag>>
	{
		size_t operator()( const wv::sStrongID<T, Tag>& _type ) const { return hash<T>()( _type.value ); }
	};

}

///////////////////////////////////////////////////////////////////////////////////////

#define WV_DEFINE_ID( _c ) typedef wv::sStrongID<uint16_t, struct _c##_t> _c
