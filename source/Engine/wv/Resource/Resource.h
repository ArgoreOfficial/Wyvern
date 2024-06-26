#pragma once

#include <string>
#include <wv/Types.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{
	/// TODO: forward declare iFileSystem for resource loading

	class iResource
    {
	public: 
		
		iResource( const std::string& _name, const std::wstring& _path ) :
			m_name{_name},
			m_path{_path}
		{ }

		/// TODO: impl? might place in iResourceRegistry
		void load  ( void ) { }
		void unload( void ) { }
		void reload( void ) { }
		
		void create ( void ) { }
		void destroy( void ) { }

		void incrNumUsers( void ) { m_numUsers++; }
		void decrNumUsers( void ) { if( m_numUsers > 0) m_numUsers--; }

		void setHandle  ( const wv::Handle& _handle ) { m_handle = _handle; }
		void setFullPath( const std::wstring& _path ) { m_path = _path; }
		
		wv::Handle   getHandle  ( void ) { return m_handle; }
		unsigned int getNumUsers( void ) { return m_numUsers; }

		std::string getName() { return m_name; }
///////////////////////////////////////////////////////////////////////////////////////

	protected:

		std::string m_name;
		std::wstring m_path;

		wv::Handle m_handle = 0;
		unsigned int m_numUsers = 0;

    };
}