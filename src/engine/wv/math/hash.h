#include <string>
#include <stdint.h>

namespace wv {
namespace Hash{

// does this work in C++11?

static constexpr uint32_t djb2( const std::string_view& _str )
{
    uint32_t hash = 5381;
    for( const auto& c : _str )
        hash = ( 33 * hash ) ^ c;
    return hash;
}

static constexpr uint32_t sdbm( const std::string_view& _str )
{
    uint32_t hash = 0;
    for( const auto& c : _str )
        hash = c + ( hash << 6 ) + ( hash << 16 ) - hash;
    return hash;
}

}
}