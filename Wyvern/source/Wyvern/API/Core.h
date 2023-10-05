#pragma once

#ifdef WYVERN_BUILD_DLL
	#define WYVERN_API __declspec(dllexport)
#else
	#define WYVERN_API  __declspec(dllimport)
#endif