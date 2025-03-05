#pragma once

#include "runtime_class.h"

#include <unordered_map>
#include <string>

namespace wv {

struct RuntimeProperties
{
    RuntimeProperties( RuntimeClass* _class ) : 
        m_class{ _class },
        m_ptrs { }
    {
        _class->queryProperties( this );
    }

    void add( const std::string& _name, void* _ptr ) {
        m_ptrs.emplace( _name, _ptr );
    }
    
    template<typename _Ty>
    _Ty get( const std::string& _name ) {
        if( m_ptrs.count( _name ) == 0 )
            return _Ty{}; // throw warning
        return *reinterpret_cast<_Ty*>( m_ptrs.at( _name ) );
    }

    template<typename _Ty>
    void set( const std::string& _name, const _Ty& _value ) {
        if( m_ptrs.count( _name ) == 0 )
            return; // throw warning
        *reinterpret_cast<_Ty*>( m_ptrs.at( _name ) ) = _value;
    }

private:
    RuntimeClass* m_class;
    std::unordered_map<std::string, void*> m_ptrs;
};

class SomeRuntimeClass : public RuntimeClass
{
public:
    void queryProperties( RuntimeProperties* _pOutProperties ) override {
        _pOutProperties->add( "foo", &foo );
        _pOutProperties->add( "bar", &bar );
        _pOutProperties->add( "fbar", &fbar );
    }

    int   foo  = 2;
    int   bar  = 42;
    float fbar = 3.0f;
};

}

