#pragma once

#include <vector>

namespace wv {

struct RemoteDebugProperty
{
	const char* name;
	void* ptr;
	size_t lastUpdatedFrame = 0;
};

class RemoteClient
{
public:
	
protected:
	std::vector<RemoteDebugProperty> m_properties;

};

}
