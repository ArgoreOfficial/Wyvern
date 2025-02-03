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
