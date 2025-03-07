#pragma once

#include <wv/string.h>

#include <vector>
#include <string>

namespace wv {

struct IRuntimeObject;

struct IRuntimeCallableBase
{
	void operator()( wv::IRuntimeObject* _obj, const std::vector<std::string>& _args ) {
		call( _obj, _args );
	}
	virtual void call( wv::IRuntimeObject* _obj, const std::vector<std::string>& _args ) = 0;
	virtual int getArgCount() = 0;
};

template<typename... _Args>
struct IRuntimeMemberCallable : IRuntimeCallableBase
{
private:

	template<std::size_t... _S>
	void _handleCall( wv::IRuntimeObject* _obj, std::index_sequence<_S...>, const std::vector<std::string>& _args );

public:
	typedef void( wv::IRuntimeObject::* fptr_t )( _Args... );
	IRuntimeMemberCallable( fptr_t _fptr ) :
		fptr{ _fptr }
	{}

	template<typename _Ty>
	IRuntimeMemberCallable( void( _Ty::* _fptr )( _Args... ) ) :
		fptr{ (fptr_t)_fptr }
	{}

	virtual int getArgCount() override { return (int)sizeof...( _Args ); }

	virtual void call( wv::IRuntimeObject* _obj, const std::vector<std::string>& _args ) override {
		_handleCall( _obj, std::index_sequence_for<_Args...>{}, _args );
	}

	fptr_t fptr;
};

template<typename ..._Args>
template<std::size_t ..._S>
void IRuntimeMemberCallable<_Args...>::_handleCall( wv::IRuntimeObject* _obj, std::index_sequence<_S...>, const std::vector<std::string>& _args )
{
	if( sizeof...( _Args ) != (int)_args.size() )
	{
		printf( "Error: Expected %i arguments, got %i", (int)getArgCount(), (int)_args.size() );
		return;
	}

	( _obj->*fptr )( wv::stot<_Args>( _args.data()[ _S ] )... );
}

}
