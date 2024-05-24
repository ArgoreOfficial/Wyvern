#pragma once
#include <stdint.h>

namespace lo 
{
    enum eCmdType
    {
        LO_CMD_TYPE_CLEAR = 0,
        LO_CMD_TYPE_BIND_SHADER,
        LO_CMD_TYPE_BIND_VERTEX_ARRAY_OBJECT,
        LO_CMD_TYPE_BIND_DRAW,
        LO_CMD_TYPE_BIND_SWAP
    };

    struct sCmd
    {
        eCmdType type;
		uint32_t idata;
        void* pdata;
    };
}