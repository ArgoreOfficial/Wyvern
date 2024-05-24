#pragma once

#include <lo/types.h>
#include <lo/Primitive/Buffer.h>

#include <stdint.h>

namespace lo
{
	
	/**
	 * u: unused
	 * s: shader
	 * a: alpha
	 * c: cullmode
	 * 
	 * uuuuu cc a ssssssss
	 *     B  9 8        0
	 **/

	#define PIPELINE_STATE_MASK_SHADER    0b0000000011111111
	#define PIPELINE_STATE_MASK_ALPHA     0b0000000100000000
	#define PIPELINE_STATE_MASK_CULLMODE  0b0000011000000000

	#define PIPELINE_STATE_BIT_SHADER   0x0
	#define PIPELINE_STATE_BIT_ALPHA    0x8
	#define PIPELINE_STATE_BIT_CULLMODE 0x9


	enum eDrawMode
	{
		LO_PIPELINE_DRAW_MODE_ARRAY,
		LO_PIPELINE_DRAW_MODE_ELEMENTS
	};

	enum eCullFace
	{
		LO_CULL_NONE = 0,
		LO_CULL_FRONT,
		LO_CULL_BACK,
		LO_CULL_FRONT_AND_BACK
	};

	struct sPipelineStateDesc
	{
		eDrawMode drawMode;
		ePrimitiveTypes type;
		eCullFace cullFace;

		sBufferDesc* vertexBufferDesc = nullptr;
		sBufferDesc* indexBufferDesc = nullptr;

		int program = 0; /// TODO:
		bool hasAlpha = false;
	};

	class cPipelineState
	{
	public:
		uint16_t state;
	};
}