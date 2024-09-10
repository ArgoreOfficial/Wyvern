#pragma once

#include <wv/Types.h>

#include <string>
#include <wv/Debug/Print.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{
	/// TODO: forward declare iFileSystem for resource loading

	class cFileSystem;
	class iGraphicsDevice;

	enum eResourceLoadState
	{
		WV_LOAD_STATE_NONE   = 0x0,
		WV_LOAD_STATE_DATA   = 0x1,
		WV_LOAD_STATE_OBJECT = 0x2,

		WV_LOAD_STATE_DATA_OBJECT = WV_LOAD_STATE_DATA | WV_LOAD_STATE_OBJECT,
	};

	class iResource
    {
	public: 
		
		iResource( const std::string& _name, const std::wstring& _path )
		{ 
			m_name = _name;
			m_path = _path;
		}

		virtual ~iResource() {};

		virtual void load  ( cFileSystem* _pFileSystem, iGraphicsDevice* _pGraphicsDevice ) = 0;
		virtual void unload( cFileSystem* _pFileSystem, iGraphicsDevice* _pGraphicsDevice ) = 0;

		virtual void reload( void ) { }
		
		void incrNumUsers( void ) { m_numUsers++; }
		void decrNumUsers( void ) { if( m_numUsers > 0) m_numUsers--; }

		void setHandle  ( const wv::Handle& _handle ) { m_handle = _handle; }
		void setFullPath( const std::wstring& _path ) { m_path = _path; }
		void setComplete( bool _complete )            { m_complete = _complete; }
		
		wv::Handle   getHandle  ( void ) { return m_handle; }
		unsigned int getNumUsers( void ) { return m_numUsers; }
		std::string  getName    ( void ) { return m_name; }

		bool isComplete ( void ) { return m_complete; }

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		
		std::string m_name;
		std::wstring m_path;

		wv::Handle m_handle = 0;
		unsigned int m_numUsers = 0;

		bool m_complete = false;
    };
}