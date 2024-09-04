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