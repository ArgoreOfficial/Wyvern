#pragma once

#include <wv/decl.h>
#include <wv/scene/scene_object.h>
#include <wv/job/job_system.h>

#include <string>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class IDeviceContext;
	class IGraphicsDevice;

///////////////////////////////////////////////////////////////////////////////////////

	class Scene
	{
	public:
		Scene( const std::string& _name, const std::string& _sourcePath = "" ) :
			m_name{ _name },
			m_sourcePath{ _sourcePath }
		{
		}

		~Scene() {
			destroyAllEntities();
		}
		
		std::string getSourcePath( void ) { return m_sourcePath; }
		std::string getName      ( void ) { return m_name; }
		
		void addChild( IEntity* _node, bool _triggerLoadAndCreate = false );
		void removeChild( IEntity* _node );
		
		void destroyAllEntities();

		void onUpdateTransforms();

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		std::string m_name = "Scene";
		std::string m_sourcePath = "";

		std::vector<IEntity*> m_entities;

	};

}