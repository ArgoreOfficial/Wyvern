#pragma once

#include <wv/Math/Transform.h>
#include <wv/Types.h>

#include <stdint.h>
#include <vector>
#include <string>

#include <wv/Engine/Engine.h>
#include <wv/JobSystem/JobSystem.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class iDeviceContext;
	class iLowLevelGraphics;

///////////////////////////////////////////////////////////////////////////////////////

	class iSceneObject
	{
	public:
		friend class Scene;

		iSceneObject( const UUID& _uuid, const std::string& _name );
		virtual ~iSceneObject() = 0;

		wv::iSceneObject* getChildByUUID( const wv::UUID& _uuid );

		std::string getName( void ) { return m_name; }
		uint64_t    getUUID( void ) { return m_uuid; }
		
	#ifdef WV_EDITOR
		bool isEditorSelected() { return m_editorSelected; }
	#endif
		
		Transformf m_transform;
		
///////////////////////////////////////////////////////////////////////////////////////

	protected:

		virtual void onLoadImpl() = 0;
		virtual void onUnloadImpl() = 0;

		virtual void onCreateImpl() = 0;
		virtual void onDestroyImpl() = 0;

		virtual void onUpdate( double _deltaTime ) = 0;
		virtual void drawImpl( wv::iDeviceContext* _context, wv::iLowLevelGraphics* _device ) = 0;

		uint64_t    m_uuid;
		std::string m_name;
		
	#ifdef WV_EDITOR
		bool m_editorSelected = false;
	#endif

	private:
		bool m_loaded  = false;
		bool m_created = false;

	};

}

// REFLECTNODE( INode )