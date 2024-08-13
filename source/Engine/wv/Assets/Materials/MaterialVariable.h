#pragma once

#include <string>

#include <wv/Types.h>
#include <wv/Misc/Color.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

    class Texture;

    ///////////////////////////////////////////////////////////////////////////////////////

    enum eMaterialVariableType
    {
        WV_MATERIAL_VARIABLE_INT,
        WV_MATERIAL_VARIABLE_FLOAT,
        WV_MATERIAL_VARIABLE_TEXTURE,
        WV_MATERIAL_VARIABLE_RGBA
    };

    union uMaterialData
    {
        int i;
        float f;
        Texture* texture;
        cColor rgba;
    };

    struct sMaterialVariable
    {
        wv::Handle handle;
        std::string name;

        eMaterialVariableType type;
        uMaterialData data;
    };

}

