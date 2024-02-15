#pragma once

#include <vector>

namespace cm
{
	class cVertexLayout
	{
	public:
		enum eType 
		{
			Float,
			Uint,
			Byte,
			Double
		};

		struct sLayoutElement
		{
			eType type;
			unsigned int count;
			size_t size;
		};

		 cVertexLayout() { }
		~cVertexLayout() { }

		template<class T>
		void push( unsigned int _count );

		std::vector<sLayoutElement> getElements() { return m_elements; }
		unsigned int getStride() { return m_stride; }

	private:
		unsigned int m_stride = 0;
		std::vector<sLayoutElement> m_elements;
	};

	template<>
	inline void cVertexLayout::push<float>( unsigned int _count )
	{
		size_t size = sizeof( float );
		m_stride += (unsigned int)size * _count;
		m_elements.push_back( { eType::Float, _count, size } );
	}

	template<>
	inline void cVertexLayout::push<unsigned char>( unsigned int _count )
	{
		size_t size = sizeof( unsigned char );
		m_stride += (unsigned int)size * _count;
		m_elements.push_back( { eType::Byte, _count, size } );
	}

	template<>
	inline void cVertexLayout::push<unsigned int>( unsigned int _count )
	{
		size_t size = sizeof( unsigned int );
		m_stride += (unsigned int)size * _count;
		m_elements.push_back( { eType::Uint, _count, size } );
	}

	template<>
	inline void cVertexLayout::push<double>( unsigned int _count )
	{
		size_t size = sizeof( double );
		m_stride += (unsigned int)size * _count;
		m_elements.push_back( { eType::Double, _count, size } );
	}
}

