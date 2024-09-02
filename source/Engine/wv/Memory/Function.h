#pragma once

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	template< class R, class...Args >
	class Function
	{
	public:
		typedef R( *fptr_t )( Args... );

		R operator()( Args..._args ) { return m_fptr( _args... ); }

		void bind( R( *_func )( Args... ) ) { m_fptr = _func; }

///////////////////////////////////////////////////////////////////////////////////////

		// R( *m_fptr )( Args... );
		fptr_t m_fptr = nullptr;
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

}
