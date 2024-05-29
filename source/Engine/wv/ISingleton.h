#pragma once

#include <wv/Decl.h>

template<class T>
class iSingleton
{
WV_DECLARE_INTERFACE( iSingleton<T> )

public:
	virtual void create() = 0;
	
	static void destroy()
	{
		delete m_instance;
		m_instance = nullptr;
	}

	static T& getInstance( void ) 
	{
		if ( !m_instance )
			m_instance = new T();
		
		return *m_instance; 
	}

protected:
	inline static T* m_instance;
};
