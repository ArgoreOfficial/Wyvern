#pragma once

#include <wv/Scene/iSceneObject.h>
#include <wv/Graphics/cModel.h>

namespace wv
{
	class cMeshObject : public iSceneObject
	{
	public:
		 cMeshObject( std::string _name );
		~cMeshObject() { }

		void create( std::string _path );

		void update( double _delta_time ) override;
		void render() override;

	private:
		wv::cModel* m_model;

	};
}