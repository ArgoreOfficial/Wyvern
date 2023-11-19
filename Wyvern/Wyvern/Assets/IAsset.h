#pragma once
#include <string>

namespace WV
{
	class iAsset
	{
	public:
		iAsset( std::string _path ): m_ready(false), m_path( _path ) { }
		~iAsset() { }

		virtual void load() { }
		bool isReady() { return m_ready; }
		std::string getPath() { return m_path; }
	protected:
		bool m_ready;
		std::string m_path;
	};
}