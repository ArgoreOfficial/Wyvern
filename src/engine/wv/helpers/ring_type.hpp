#pragma once

namespace wv {

template<typename _Ty, _Ty _Start, _Ty _End>
struct ring_type
{
    // prefix increment
    _Ty& operator++() {
        ++m_val;
        // wrap
        if( m_val >= _End )
            m_val = _Start;
        return m_val;
    }

    // postfix increment
    _Ty operator++( _Ty ) {
        _Ty old = m_val; // copy old value
        operator++();    // prefix increment
        return old;      // return old value
    }

    _Ty& operator+=( const _Ty& _rhs ) {
        m_val += _rhs;

    }

    // prefix decrement
    _Ty& operator--() {
        --m_val;
        // wrap
        if( m_val < _Start )
            m_val = _End - 1;
        return m_val;
    }

    // postfix decrement
    _Ty operator--( _Ty ) {
        _Ty old = m_val; // copy old value
        operator--();    // prefix decrement
        return old;      // return old value
    }

private:
	_Ty m_val;
};

}
