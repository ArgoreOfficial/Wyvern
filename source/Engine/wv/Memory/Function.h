#pragma once

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	template< class R, class...Args >
	class Function
	{
	public:
		void operator()( Args..._args ) { m_func( _args... ); }

		void bind( R( *_func )( Args... ) ) { m_func = _func; }

///////////////////////////////////////////////////////////////////////////////////////

	private:
		R( *m_func )( Args... );

	};

///////////////////////////////////////////////////////////////////////////////////////

	template< class T, class R, class...Args >
	class MemberFunction
	{
	public:
		
		void operator()( Args..._args ) { ( m_class->*m_func )( _args... ); }

		void bind( R( T::* _func )( Args... ), T* _member ) { m_func = _func; m_class = _member; }

///////////////////////////////////////////////////////////////////////////////////////

	private:
		R( T::* m_func )( Args... );
		T* m_class;
	};

///////////////////////////////////////////////////////////////////////////////////////

}