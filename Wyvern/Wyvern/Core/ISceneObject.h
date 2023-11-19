#pragma once

namespace WV
{
	class iSceneObject
	{
	public:
		iSceneObject() { }
		~iSceneObject() { }
		
		inline void setID( unsigned int _id ) 
		{ 
			if ( m_internalID == 0 ) 
				m_internalID = _id; 
		}

		virtual void update( double _deltaTime ) { }
		virtual void draw3D() { }
		virtual void draw2D() { }
		virtual void drawUI() { }

	protected:
		unsigned int m_internalID = 0;
	};
}