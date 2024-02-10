#pragma once

template<class T>
class iSingleton
{
public:
	iSingleton<T>( const iSingleton<T>& ) = delete;

	virtual void onCreate() { }

	static T& getInstance( void ) 
	{
		if ( !m_instance )
		{
			m_instance = new T();
			m_instance->onCreate();
		}

		return *m_instance; 
	}

protected:
	iSingleton<T>( void ) { }

	inline static T* m_instance;
};
