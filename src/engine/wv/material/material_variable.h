#pragma once

#include <string>

#include <wv/types.h>
#include <wv/misc/color.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

    class TextureResource;

    ///////////////////////////////////////////////////////////////////////////////////////

    enum MaterialVariableType
    {
        WV_MATERIAL_VARIABLE_INT,
        WV_MATERIAL_VARIABLE_FLOAT,
        WV_MATERIAL_VARIABLE_TEXTURE,
        WV_MATERIAL_VARIABLE_RGBA
    };

    union MaterialData
    {
        int i;
        float f;
        TextureResource* texture;
        Color* rgba;
    };

    struct MaterialVariable
    {
        wv::Handle handle;
        std::string name;

        MaterialVariableType type;
        MaterialData data;
    };

}

