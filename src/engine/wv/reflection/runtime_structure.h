#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace wv {

class RuntimeStructure
{
public:
	struct MemberSpan
	{
		std::string name;
		uint32_t size;
		uint32_t offset;
	};

	RuntimeStructure() = default;

	void pushSpan( const std::string& _name, uint32_t _size );

	size_t getSize() const { return m_bufferSize; }
	size_t getMemberOffset( const std::string& _name ) const;

private:
	size_t m_bufferSize = 0;
	std::vector<MemberSpan> m_members = {};
	std::unordered_map<std::string, size_t> m_memberNameMap = {};
};

}