#pragma once

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	template< class R, class...Args >
	class Function
	{
	public:
		typedef R( *fptr_t )( Args... );

		Function()                      : m_fptr{ nullptr } {}
		Function( const fptr_t& _func ) : m_fptr{ _func }   {}

		R operator()( Args..._args ) { return m_fptr( _args... ); }

		void bind( const fptr_t& _func ) { m_fptr = _func; }
		
		template<typename T>
		void bind( T& t );

///////////////////////////////////////////////////////////////////////////////////////
		fptr_t m_fptr = nullptr;

	private:

		template<typename T>
		static void* lambdaFunc( void* new_fn = nullptr )
		{
			static void* fn;
			if ( new_fn != nullptr )
				fn = new_fn;
			return fn;
		}

		template<typename T>
		static R lambdaFptr( Args... _args )
		{
			T* tPtr = (T*)lambdaFunc<T>();
			return (R)(*tPtr)( _args... );
		}
	};

///////////////////////////////////////////////////////////////////////////////////////

	template< class T, class R, class...Args >
	class MemberFunction
	{
	public:
		
		typedef R( T::* mfptr_t )( Args... );

		R operator()( Args..._args ) { return ( m_class->*m_fptr )( _args... ); }

		void bind( R( T::* _func )( Args... ), T* _member ) { m_fptr = _func; m_class = _member; }

///////////////////////////////////////////////////////////////////////////////////////

		// R( T::* m_func )( Args... );
		mfptr_t m_fptr = nullptr;
		T* m_class;
	};

///////////////////////////////////////////////////////////////////////////////////////

	template<class R, class ...Args>
	template<typename T>
	inline void Function<R, Args...>::bind( T& t )
	{
		lambdaFunc<T>( &t );
		m_fptr = (fptr_t)lambdaFptr<T>;
	}

}
