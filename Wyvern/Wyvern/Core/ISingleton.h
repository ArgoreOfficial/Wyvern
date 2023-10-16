#pragma once

namespace WV
{
	template <class T>
	class ISingleton
	{
	public:
		ISingleton<T>( const ISingleton<T>& ) = delete;

		static T& getInstance()
		{
			static T instance;
			return instance;
		}
		
	protected:
		ISingleton<T>() { }
	};
}