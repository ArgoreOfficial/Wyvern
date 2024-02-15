#pragma once

/*
 *
 * Buffer.h
 * 
 * Type definitions for buffer handles
 * 
 */
namespace cm
{
	typedef unsigned int hBuffer;

	enum eBufferType
	{
		BufferType_Vertex,
		BufferType_Index
	};

	struct sBuffer
	{
		hBuffer handle;
		eBufferType type;
	};
}