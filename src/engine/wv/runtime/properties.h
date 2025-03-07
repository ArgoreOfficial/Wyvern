#pragma once

#include <wv/runtime/object.h>
#include <wv/runtime/callable.h>

#include <unordered_map>
#include <string>

namespace wv {

struct IRuntimeCallableBase;

struct RuntimeFunctions
{
    RuntimeFunctions() :
        m_ptrs{}
    {}

    std::vector<std::string> list()
    {
        std::vector<std::string> funcs;
        for( auto& f : m_ptrs )
            funcs.push_back( f.first );
        return funcs;
    }

    template<typename _Ty, typename... _Args>
    void add( const std::string& _name, void(_Ty::*_fptr)( _Args... ) ) {
        IRuntimeCallableBase* fptr = new RuntimeMemberCallable{ _fptr };
        m_ptrs.emplace( _name, fptr );
    }

    IRuntimeCallableBase* getPtr( const std::string& _name ) {
        if( m_ptrs.count( _name ) == 0 )
            return nullptr; // throw warning

        return m_ptrs.at( _name );
    }

private:
    std::unordered_map<std::string, IRuntimeCallableBase*> m_ptrs;
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

    template<typename _Ty, typename _Oty>
    void add( const std::string& _name, _Ty _Oty::* _ptr ) {
        m_ptrs.emplace( _name, (uint8_t IRuntimeObject::* )_ptr );
    }

    uint8_t wv::IRuntimeObject::* getPtr( const std::string& _name ) {
        if( m_ptrs.count( _name ) == 0 )
            return nullptr; // throw warning

        return m_ptrs.at( _name );
    }

private:
    std::unordered_map<std::string, uint8_t IRuntimeObject::*> m_ptrs;
};

}
