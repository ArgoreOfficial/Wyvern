#pragma once

#include <wv/runtime/runtime_object.h>

#include <unordered_map>
#include <string>

namespace wv {

struct RuntimeMethods
{
    std::vector<const char*> methods;
};

struct RuntimeProperties
{
    RuntimeProperties() : 
        m_ptrs{}
    {}

    std::vector<std::string> list() 
    {
        std::vector<std::string> props;
        for( auto& p : m_ptrs )
            props.push_back( p.first );
        return props;
    }

    void add( const std::string& _name, uint8_t IRuntimeObjectBase::* _ptr ) {
        m_ptrs.emplace( _name, _ptr );
    }

    uint8_t wv::IRuntimeObjectBase::* getPtr( const std::string& _name ) {
        if( m_ptrs.count( _name ) == 0 )
            return nullptr; // throw warning

        return m_ptrs.at( _name );
    }

private:
    std::unordered_map<std::string, uint8_t IRuntimeObjectBase::*> m_ptrs;
};

}
