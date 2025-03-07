#pragma once

#include <wv/runtime/object.h>
#include <wv/runtime/callable.h>

#include <unordered_map>
#include <string>

namespace wv {

struct IRuntimeCallable;

enum class RuntimeMemberType // TODO
{
    kVoid,
    kInt,
    kFloat,
    kString,
};

template<typename _Ty> static RuntimeMemberType getRuntimeMemberType() { return RuntimeMemberType::kVoid; }
template<> RuntimeMemberType getRuntimeMemberType<int>        () { return RuntimeMemberType::kInt;    }
template<> RuntimeMemberType getRuntimeMemberType<float>      () { return RuntimeMemberType::kFloat;  }
template<> RuntimeMemberType getRuntimeMemberType<std::string>() { return RuntimeMemberType::kString; }


struct RuntimeMemberProperty
{
    RuntimeMemberType type;
    uint8_t IRuntimeObject::* ptr;
};

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
        IRuntimeCallable* fptr = new RuntimeMemberCallable{ _fptr };
        m_ptrs.emplace( _name, fptr );
    }

    IRuntimeCallable* getPtr( const std::string& _name ) {
        if( m_ptrs.count( _name ) == 0 )
            return nullptr; // throw warning

        return m_ptrs.at( _name );
    }

private:
    std::unordered_map<std::string, IRuntimeCallable*> m_ptrs;
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
        RuntimeMemberProperty* mp = new RuntimeMemberProperty();
        mp->ptr = ( uint8_t IRuntimeObject::* )_ptr;
        mp->type = getRuntimeMemberType<_Ty>();
        m_ptrs.emplace( _name, mp );
    }

    RuntimeMemberProperty* getPtr( const std::string& _name ) {
        if( m_ptrs.count( _name ) == 0 )
            return {}; // throw warning

        return m_ptrs.at( _name );
    }

private:
    std::unordered_map<std::string, RuntimeMemberProperty*> m_ptrs;
};

}
