#pragma once

#include <wv/types.h>

#include <string>
#include <wv/debug/log.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{
	class IFileSystem;
	class IGraphicsDevice;

	class IResource
    {
	public: 
		
		IResource( const std::string& _name, const std::string& _path )
		{ 
			m_name = _name;
			m_path = _path;
		}

		virtual ~IResource() {};

		virtual void load  ( IFileSystem* _pFileSystem, IGraphicsDevice* _pGraphicsDevice ) = 0;
		virtual void unload( IFileSystem* _pFileSystem, IGraphicsDevice* _pGraphicsDevice ) = 0;

		virtual void reload( void ) { }
		
		void incrNumUsers( void ) { m_numUsers++; }
		void decrNumUsers( void ) { if( m_numUsers > 0) m_numUsers--; }

		void setHandle  ( const wv::Handle& _handle ) { m_handle = _handle; }
		void setFullPath( const std::string& _path )  { m_path = _path; }
		void setComplete( bool _complete )            { m_complete = _complete; }
		
		wv::Handle   getHandle  ( void ) { return m_handle; }
		unsigned int getNumUsers( void ) { return m_numUsers; }
		std::string  getName    ( void ) { return m_name; }

		bool isComplete ( void ) { return m_complete; }

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		
		std::string m_name;
		std::string m_path;

		wv::Handle m_handle = 0;
		unsigned int m_numUsers = 0;

		bool m_complete = false;
    };
}