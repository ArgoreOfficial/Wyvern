#pragma once

namespace WV
{
	template <class T>
	class iSingleton
	{
	public:
		iSingleton<T>( const iSingleton<T>& ) = delete;

	protected:
		static T& getInstance()
		{
			static T instance;
			return instance;
		}
		
		iSingleton<T>() { }
	};
}