#pragma once

template<class T>
class iSingleton
{
public:
	iSingleton<T>( const iSingleton<T>& ) = delete;
	iSingleton<T>& operator= ( const iSingleton<T>& ) = delete;

	virtual void create() = 0;

	static T& getInstance( void ) 
	{
		if ( !m_instance )
			m_instance = new T();
		
		return *m_instance; 
	}

protected:
	iSingleton<T>( void ) { }

	inline static T* m_instance;
};
