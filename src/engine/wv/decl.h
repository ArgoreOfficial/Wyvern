#pragma once

///////////////////////////////////////////////////////////////////////////////////////

#define WV_DECLARE_INTERFACE( _class )               \
	public:                                          \
		virtual ~_class() { }                        \
	protected:                                       \
		_class() { }                                 \
		_class( const _class& )            = delete; \
		_class& operator=( const _class& ) = delete; \
		_class( _class && )                = delete; \
		_class& operator=( _class && )     = delete; \
	private:

///////////////////////////////////////////////////////////////////////////////////////

// to avoid int -> void* warnings
#define VPTRi32(V) reinterpret_cast<void*>( static_cast<size_t>(V))

///////////////////////////////////////////////////////////////////////////////////////

#define WV_ENUM_BITWISE_OR_TYPE( _Ty, _Bty ) static inline _Ty operator|( _Ty _a, _Ty _b ) { \
	return static_cast<_Ty>( static_cast<_Bty>( _a ) | static_cast<_Bty>( _b ) ); \
}
#define WV_ENUM_BITWISE_OR( _Ty ) WV_ENUM_BITWISE_OR_TYPE( _Ty, int )

///////////////////////////////////////////////////////////////////////////////////////

/**

Pad structure to specified size
Usage:

struct foo {
	int   bar0;
	float bar1;
	char  bar2;

	WV_PAD_PAYLOAD( sizeof(bar0) + sizeof(bar1) + sizeof(bar2) );
	WV_PAD_TO_T(64);
};

*/

#define WV_PAD_PAYLOAD( _size ) static constexpr size_t _PAYLOAD_SIZE = _size ;
//#define WV_PAD_TO_T( _size ) std::array<unsigned char, _size - _PAYLOAD_SIZE>
#define WV_PAD_TO_T( _size ) struct pad_t { unsigned char m[_size - _PAYLOAD_SIZE]; }

///////////////////////////////////////////////////////////////////////////////////////
