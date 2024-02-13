#pragma once

#include <vector>
#include <string>

namespace wv
{
	class iSceneObject;

	class cScene
	{
	public:
		 cScene( std::string _name ) : m_name{ _name } { }
		~cScene( void ) { }

		void create ( void );
		void destroy( void );
    
		void load  ( void );
		void unload( void );

		void update( double _delta_time );
		void render( void );

		std::string getName( void ) { return m_name; }
		
		void addObject( iSceneObject* _object );

	private:

		bool m_paused = false;
		std::string m_name;

		std::vector<iSceneObject*> m_objects{};
	};

}
