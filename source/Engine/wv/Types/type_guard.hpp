#pragma once

#include <mutex>
#include <atomic>

namespace arx {

template<typename _Ty>
struct type_guard
{
    type_guard() :
        m_value{ 0 }
    {}

    type_guard( const _Ty& _v ) :
        m_value{ _v }
    {}

    type_guard( const type_guard<_Ty>& _v ) :
        m_value{ _v.m_value }
    {}

    _Ty* lock() {
        m_mutex.lock();
        return &m_value;
    }

    bool try_lock( _Ty*& _out ) {
        bool locked = m_mutex.try_lock();
        _out = &m_value;
        return locked;
    }

    void unlock() {
        m_mutex.unlock();
    }

private:
    std::mutex m_mutex{};
    _Ty m_value{};
};

}

