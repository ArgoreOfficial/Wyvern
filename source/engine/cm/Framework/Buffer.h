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
		BufferType_Index,
		BufferType_Uniform
	};

	enum eBufferUsage
	{
		BufferUsage_Dynamic,
		BufferUsage_Static
	};

	struct sBuffer
	{
		hBuffer handle;
		eBufferType type;
		eBufferUsage usage;
	};
}