#pragma ocne

namespace WV
{
	class ISceneObject
	{
	public:
		~ISceneObject() { }
		
		inline void setID( unsigned int _id ) 
		{ 
			if ( m_internalID == 0 ) 
				m_internalID = _id; 
		}

	protected:
		ISceneObject() { }
		unsigned int m_internalID = 0;
	};
}